import os, pickle, math, time
import torch
from torch import nn
from torch.utils.data import DataLoader

from chess_ai.data.tokenizer import Tokenizer
from chess_ai.settings import *
from chess_ai.training.model import Model
from chess_ai.training.optimizer import AdamW
from chess_ai.testing.evaluate import estimate_elo


class Trainer():

	def __init__(self, model: Model, tokenizer: Tokenizer, train_loader: DataLoader, val_loader: DataLoader, train_size: int):

		self.model = model
		model.train()

		self.tokenizer = tokenizer
		self.train_loader = train_loader
		self.val_loader = val_loader
		self.train_size = train_size

		self.time = None
		self.step = 0
		self.tokens = 0
		self.epochs = 0.0
		self.learning_rate = 0.0
		self.loss = 0.0
		self.accuracy = 0.0
		self.val_loss = 0.0
		self.val_accuracy = 0.0
		self.loss_ema = None
		self.accuracy_ema = None
		self.best_val_loss = float('inf')
		self.elo = 0

		self.optimizer = AdamW(self.model.parameters(), self.learning_rate)

		self.metrics_history = {
			'time': [],
			'step': [],
			'tokens': [],
			'epochs': [],
			'loss': [],
			'accuracy': [],
			'val_loss': [],
			'val_accuracy': [],
			'elo': []
		}


	def save_model(self, path: str) -> None:

		if not os.path.exists(path):
			os.makedirs(path)

		torch.save(self.model.state_dict(), os.path.join(path, 'model.pt'))
		torch.save(self.optimizer.state_dict(), os.path.join(path, 'optimizer.pt'))


	def load_model(self, path) -> None:

		if not os.path.exists(path):
			return

		self.model.load_state_dict(torch.load(os.path.join(path, 'model.pt'), map_location=DEVICE))
		self.optimizer.load_state_dict(torch.load(os.path.join(path, 'optimizer.pt'), map_location=DEVICE))


	def find_previous_session(self) -> None:

		if os.path.exists(os.path.join(OUTPUT_DIR, 'last')):
			self.load_model(os.path.join(OUTPUT_DIR, 'last'))

		if os.path.exists(os.path.join(OUTPUT_DIR, 'metrics.pkl')):
			self.load_metrics()


	def print(self) -> None:

		print(f'Epochs: {self.epochs:.4f} | Steps: {self.step:,} | Tokens: {self.tokens:,} | LR: {self.learning_rate:.5f}   ||   ' \
			f'Loss: {self.loss_ema:.5f} | Accuracy: {self.accuracy_ema * 100.0:.4f} % | ' \
			f'Val loss: {self.val_loss:.5f} | Val accuracy: {self.val_accuracy * 100.0:.4f} % | ' \
			f'Elo: {self.elo}         ', end='\r')


	def save_metrics(self) -> None:

		if self.time is None:
			self.metrics_history['time'].append(0.0)
		else:
			self.metrics_history['time'].append(self.metrics_history['time'][-1] + (time.time() - self.time))

		self.time = time.time()

		self.metrics_history['step'].append(self.step)
		self.metrics_history['tokens'].append(self.tokens)
		self.metrics_history['epochs'].append(self.epochs)
		self.metrics_history['loss'].append(self.loss_ema)
		self.metrics_history['accuracy'].append(self.accuracy_ema)
		self.metrics_history['val_loss'].append(self.val_loss)
		self.metrics_history['val_accuracy'].append(self.val_accuracy)
		self.metrics_history['elo'].append(self.elo)

		if not os.path.exists(OUTPUT_DIR):
			os.makedirs(OUTPUT_DIR)

		pickle.dump(self.metrics_history, open(os.path.join(OUTPUT_DIR, 'metrics.pkl'), 'wb'))


	def load_metrics(self) -> None:

		self.metrics_history = pickle.load(open(os.path.join(OUTPUT_DIR, 'metrics.pkl'), 'rb'))

		self.step = self.metrics_history['step'][-1]
		self.tokens = self.metrics_history['tokens'][-1]
		self.epochs = self.metrics_history['epochs'][-1]
		self.loss_ema = self.metrics_history['loss'][-1]
		self.accuracy_ema = self.metrics_history['accuracy'][-1]
		self.val_loss = self.metrics_history['val_loss'][-1]
		self.val_accuracy = self.metrics_history['val_accuracy'][-1]
		self.best_val_loss = min(self.metrics_history['val_loss'])
		self.elo = self.metrics_history['elo'][-1]
		self.time = time.time()


	def update_learning_rate(self) -> None:

		if self.step < WARMUP_STEPS:
			ratio = self.step / WARMUP_STEPS
			self.learning_rate = MAX_LEARNING_RATE * ratio
		elif self.step < WARMUP_STEPS + DECAY_STEPS:
			ratio = (self.step - WARMUP_STEPS) / DECAY_STEPS
			ratio = 0.5 * (1.0 + math.cos(math.pi * ratio))
			self.learning_rate = ratio * (MAX_LEARNING_RATE - MIN_LEARNING_RATE) + MIN_LEARNING_RATE
		else:
			self.learning_rate = MIN_LEARNING_RATE

		for g in self.optimizer.param_groups:
			g['lr'] = self.learning_rate


	def apply_ema(self, value_1: float, value_2: float) -> float:

		if value_1 is None:
			return value_2

		return value_1 * METRICS_BETA + value_2 * (1.0 - METRICS_BETA)


	def train(self) -> None:

		# Training loop
		while True:

			# Update step
			self.step += 1
			self.tokens += (MAX_CONTEXT + 1) * BATCH_SIZE * NUM_ACCUMULATIONS
			self.epochs += ((MAX_CONTEXT + 1) * BATCH_SIZE * NUM_ACCUMULATIONS) / self.train_size

			# Update learning rate
			self.update_learning_rate()

			# ----- Training ----- #

			self.model.train()
			self.loss = 0.0
			self.accuracy = 0.0

			for _ in range(NUM_ACCUMULATIONS):

				# Get data
				x, y, strength = next(self.train_loader)

				x = x.to(DEVICE, non_blocking = True)
				y = [y[:, i, :].to(DEVICE, non_blocking = True) for i in range(self.tokenizer.nb_layers)]
				strength = strength.to(DEVICE, non_blocking = True)

				with CONTEXT:

					# Forward pass
					prediction = self.model(x)

					# Loss
					losses = [nn.functional.cross_entropy(
						input = prediction[i].reshape(-1, prediction[i].shape[-1]),
						target = y[i].reshape(-1),
						ignore_index = PADDING_TOKEN,
						reduction = 'none'
					) for i in range(self.tokenizer.nb_layers)]

					loss = losses[0]

					for i in range(1, len(losses)):
						loss += losses[i]

					loss = ((loss * strength.reshape(-1)).sum() / (strength.sum() + 1e-8)) / NUM_ACCUMULATIONS
					self.loss += loss.item()

					# Accuracy
					accuracies = [(prediction[i].argmax(dim = 2) == y[i]).to(dtype = torch.float32) for i in self.tokenizer.useful_layers]
					accuracy = accuracies[0]

					for i in range(1, len(accuracies)):
						accuracy *= accuracies[i]

					self.accuracy += (((accuracy * strength).sum() / (strength.sum() + 1e-8)) / NUM_ACCUMULATIONS).item()

				# Backward pass
				loss.backward()

			# Update weights
			self.model.clean_nan()
			self.model.clip_gradient(CLIP_GRADIENT)
			self.optimizer.step()
			self.optimizer.zero_grad(set_to_none = True)

			# Update ema values
			self.loss_ema = self.apply_ema(self.loss_ema, self.loss)
			self.accuracy_ema = self.apply_ema(self.accuracy_ema, self.accuracy)

			# ----- Validations ----- #

			if self.step % VAL_INTERVAL == 0:

				self.model.eval()

				with torch.no_grad():

					self.val_loss = 0.0
					self.val_accuracy = 0.0

					for _ in range(NUM_ACCUMULATIONS):

						# Get data
						x, y, strength = next(self.val_loader)

						x = x.to(DEVICE, non_blocking = True)
						y = [y[:, i, :].to(DEVICE, non_blocking = True) for i in range(self.tokenizer.nb_layers)]
						strength = strength.to(DEVICE, non_blocking = True)

						with CONTEXT:

							# Forward pass
							prediction = self.model(x)

							# Loss
							losses = [nn.functional.cross_entropy(
								input = prediction[i].reshape(-1, prediction[i].shape[-1]),
								target = y[i].reshape(-1),
								ignore_index = PADDING_TOKEN,
								reduction = 'none'
							) for i in range(self.tokenizer.nb_layers)]

							loss = losses[0]

							for i in range(1, len(losses)):
								loss += losses[i]

							self.val_loss += (((loss * strength.reshape(-1)).sum() / (strength.sum() + 1e-8)) / NUM_ACCUMULATIONS).item()

							# Accuracy
							accuracies = [(prediction[i].argmax(dim = 2) == y[i]).to(dtype = torch.float32) for i in self.tokenizer.useful_layers]
							accuracy = accuracies[0]

							for i in range(1, len(accuracies)):
								accuracy *= accuracies[i]

							self.val_accuracy += (((accuracy * strength).sum() / (strength.sum() + 1e-8)) / NUM_ACCUMULATIONS).item()

				# Update elo
				if self.step % ELO_INTERVAL == 0:
					self.elo = estimate_elo(self.model, self.tokenizer, self.elo, verbose = False)

				# Save
				self.save_metrics()
				self.save_model(os.path.join(OUTPUT_DIR, 'last'))

				# Save best
				if self.val_loss <= self.best_val_loss:
					self.best_val_loss = self.val_loss
					self.save_model(os.path.join(OUTPUT_DIR, 'best'))

			# -------------------- #

			# Print
			self.print()

			# Stop training
			if STOP_TRAINING and self.step >= WARMUP_STEPS + DECAY_STEPS:
				return
