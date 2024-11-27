import torch, random
from chess import Move, Board

from chess_ai.training.model import Model
from chess_ai.data.tokenizer import Tokenizer
from chess_ai.settings import *


class Sampler():

	def __init__(self, model: Model, tokenizer: Tokenizer, bot_color: str):

		self.model = model
		self.tokenizer = tokenizer

		self.reset(bot_color)


	def reset(self, bot_color: str, nb_random_moves: int = 0) -> None:

		self.bot_color = bot_color
		self.input = self.tokenizer.encode_game_start(bot_color)
		self.moves: list[Move] = []
		self.board = Board()

		self.random_first_move(nb_random_moves)

		if bot_color == 'white':
			self.bot_turn()


	def random_first_move(self, nb: int = 0) -> Move:

		for _ in range(nb * 2):

			move = random.choice(list(self.board.legal_moves))

			while self.board.san(move).endswith('#'):
				move = random.choice(list(self.board.legal_moves))

			self.apply_move(move)


	def apply_move(self, move: Move) -> None:

		self.input.extend(self.tokenizer.encode_move(move))
		self.moves.append(move)
		self.board.push(move)


	def bot_turn(self) -> Move | None:

		possible_moves = list(self.board.legal_moves)

		if len(possible_moves) == 0 or self.board.is_game_over():
			return

		moves = [self.tokenizer.encode_move(move) for move in possible_moves]

		if len(possible_moves) == 1:
			self.apply_move(possible_moves[0])
			return possible_moves[0]

		for possible_move in possible_moves:
			if self.board.san(possible_move).endswith('#'):
				self.apply_move(possible_move)
				return possible_move

		max_move_length = max(len(move) for move in moves)
		max_length = len(self.input) + max_move_length
		inputs = []

		if max_length > MAX_CONTEXT:
			self.input = self.input[-(MAX_CONTEXT - max_move_length):]
			max_length = MAX_CONTEXT

		for move in moves:

			input = self.input.copy()
			input.extend(move)
			input.extend([PADDING_TOKEN] * (max_length - len(input)))
			inputs.append(input[:-1])

		with torch.no_grad(), CONTEXT:

			input_tensor = torch.tensor(inputs, dtype = torch.long, device = DEVICE)
			output: torch.Tensor = self.model(input_tensor)

		output = output.float().detach().to('cpu')
		output = output[:, len(self.input) - 1:]
		output = torch.softmax(output, dim = -1)

		move_likelihoods = []

		for i, move in enumerate(moves):

			probabilities = output[i, list(range(len(move))), move]
			likelihood = torch.prod(probabilities).item()
			move_likelihoods.append(likelihood)

		best = torch.argmax(torch.tensor(move_likelihoods)).item()
		final_move = possible_moves[best]
		self.apply_move(final_move)

		return final_move


	def play(self, move: Move, verbose: bool = True) -> bool:

		self.input.extend(self.tokenizer.encode_move(move))
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
