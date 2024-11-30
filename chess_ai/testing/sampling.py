import torch, random
import numpy as np
from chess import Move, Board

from chess_ai.training.model import Model
from chess_ai.data.tokenizer import Tokenizer
from chess_ai.settings import *


class Sampler():

	def __init__(self, model: Model, tokenizer: Tokenizer, bot_color: str):

		self.model = model
		self.tokenizer = tokenizer

		self.reset(bot_color)


	def reset(self, bot_color: str, nb_random_moves: int = 0, firt_bot_move: bool = True) -> None:

		self.bot_color = bot_color
		self.input = self.tokenizer.encode_game_start(bot_color)
		self.moves: list[Move] = []
		self.board = Board()

		self.random_first_move(nb_random_moves)

		if bot_color == 'white' and firt_bot_move:
			self.bot_turn()


	def random_first_move(self, nb: int = 0) -> Move:

		for _ in range(nb * 2):

			move = random.choice(list(self.board.legal_moves))

			while self.board.san(move).endswith('#'):
				move = random.choice(list(self.board.legal_moves))

			self.apply_move(move)


	def apply_move(self, move: Move) -> None:

		self.input = np.concatenate([self.input, self.tokenizer.encode_move(move, self.board)], axis = 1)
		self.moves.append(move)
		self.board.push(move)


	def bot_turn(self) -> Move | None:

		# Get all possible moves
		possible_moves = list(self.board.legal_moves)

		# No possible move case
		if len(possible_moves) == 0 or self.board.is_game_over():
			return

		# Only one possible move case
		if len(possible_moves) == 1:
			self.apply_move(possible_moves[0])
			return possible_moves[0]

		# Checkmate if possible
		for possible_move in possible_moves:
			if '#' in self.board.san(possible_move):
				self.apply_move(possible_move)
				return possible_move

		# Encode possible moves
		moves = [self.tokenizer.encode_move(move, self.board) for move in possible_moves]

		# Only keep the useful part of the moves
		for i in range(len(moves)):
			if moves[i].shape[1] > self.tokenizer.min_useful_length:
				moves[i] = moves[i][:, :self.tokenizer.min_useful_length]

		# Add previous moves to the model input
		max_length = self.input.shape[1] + self.tokenizer.min_useful_length
		inputs = []

		if max_length > MAX_CONTEXT:
			self.input = self.input[:, -(MAX_CONTEXT - self.tokenizer.min_useful_length):]
			max_length = MAX_CONTEXT

		inputs = np.zeros((len(moves), self.tokenizer.nb_layers, max_length), dtype = np.uint8)

		for i in range(len(moves)):

			inputs[i, :, :self.input.shape[1]] = self.input
			inputs[i, :, self.input.shape[1]:self.input.shape[1] + moves[i].shape[1]] = moves[i]

		# Get the model outputs
		with torch.no_grad():

			input_tensor = torch.tensor(inputs.astype(np.int64), dtype = torch.long, device = DEVICE)

			with CONTEXT:
				outputs = self.model(input_tensor)
				# nb_layers (nb_moves, max_length, vocab_sizes[i])

			outputs = [outputs[i] for i in self.tokenizer.useful_layers]
			# nb_useful_layers (nb_moves, max_length, vocab_sizes[i])

			# Get the move likelihoods
			for i in range(len(outputs)):

				outputs[i] = outputs[i].float().detach().to('cpu')
				outputs[i] = outputs[i][:, self.input.shape[1] - 1:]
				outputs[i] = torch.softmax(outputs[i], dim = -1)
				# nb_useful_layers (nb_moves, max_move_length + 1, vocab_sizes[i])

				output = torch.ones(outputs[i].shape[0], outputs[i].shape[1], device = 'cpu')

				for j, move in enumerate(moves):
					output[j][:move.shape[1]] = outputs[i][j, list(range(move.shape[1])), move[i].tolist()]

				outputs[i] = output
				# nb_useful_layers (nb_moves, max_move_length + 1)

			output = outputs[0]

			for i in range(1, len(outputs)):
				output *= outputs[i]
				# (nb_moves, max_move_length + 1)

			# Get the best move
			move_likelihoods = torch.prod(output, dim = 1)
			best = torch.argmax(move_likelihoods).item()

		final_move = possible_moves[best]
		self.apply_move(final_move)

		return final_move


	def play(self, move: Move, verbose: bool = True) -> bool:

		self.input = np.concatenate([self.input, self.tokenizer.encode_move(move, self.board)], axis = 1)
		self.moves.append(move)
		self.board.push(move)

		move = self.bot_turn()

		if move is None:
			if verbose:
				print('Game over')
			return False

		if verbose:
			print(move)

		if self.board.is_game_over():
			if verbose:
				print('Game over')
			return False

		return True
