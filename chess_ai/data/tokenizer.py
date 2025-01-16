from chess_ai.settings import *
import numpy as np
import numpy.typing as npt
import chess
from chess import Board, Move


to_symbol = {
	'P': '♙',
	'N': '♘',
	'B': '♗',
	'R': '♖',
	'Q': '♕',
	'K': '♔',
	'p': '♟',
	'n': '♞',
	'b': '♝',
	'r': '♜',
	'q': '♛',
	'k': '♚'
}


class Tokenizer:

	def __init__(self) -> None:

		self.position_vocab = CONTROLS + ['white', 'black', 'draw'] + [l + str(n) for l in 'abcdefgh' for n in range(1, 9)]
		self.piece_vocab = [' ', '♔', '♕', '♖', '♗', '♘', '♙', '♚', '♛', '♜', '♝', '♞', '♟']
		self.vocab = self.position_vocab + self.piece_vocab

		self.to_position_ids = {token: i for i, token in enumerate(self.position_vocab)}
		self.to_position_tokens = {i: token for i, token in enumerate(self.position_vocab)}
		self.to_piece_ids = {token: i for i, token in enumerate(self.piece_vocab)}
		self.to_piece_tokens = {i: token for i, token in enumerate(self.piece_vocab)}

		self.nb_layers = 6
		self.layer_sizes = [len(self.position_vocab), len(self.piece_vocab), 2, 2, 2, 2]
		self.useful_layers = [0, 1]
		self.min_useful_length = 2


	def encode_move(self, move: Move | str, board: Board | None = None) -> npt.NDArray[np.uint8]:

		positions = []
		capture = False
		en_passant = False
		check = False
		checkmate = False

		if isinstance(move, str):

			parts = move.split('.')
			positions.append((parts[1][0], parts[1][1:3]))
			positions.append((parts[1][3], parts[1][4:6]))

			if len(parts[1]) > 6:
				positions.append((parts[1][6], parts[1][7:9]))
				positions.append((parts[1][9], parts[1][10:12]))

			if 'x' in parts[2]:
				capture = True

			if '*' in parts[2]:
				en_passant = True

			if '+' in parts[3]:
				check = True

			if '#' in parts[3]:
				checkmate = True

		else:

			positions.append((to_symbol[board.piece_at(move.from_square).symbol()], move.uci()[:2]))

			if move.promotion is not None:
				piece = chess.piece_symbol(move.promotion)
				positions.append((to_symbol[piece.upper()] if board.turn == chess.WHITE else to_symbol[piece.lower()], move.uci()[2:4]))
			else:
				positions.append((to_symbol[board.piece_at(move.from_square).symbol()], move.uci()[2:4]))

			if board.is_castling(move):

				if board.turn == chess.WHITE and board.is_kingside_castling(move):
					positions.append(('♖', 'h1'))
					positions.append(('♖', 'f1'))

				elif board.turn == chess.WHITE and board.is_queenside_castling(move):
					positions.append(('♖', 'a1'))
					positions.append(('♖', 'd1'))

				elif board.turn == chess.BLACK and board.is_kingside_castling(move):
					positions.append(('♜', 'h8'))
					positions.append(('♜', 'f8'))

				elif board.turn == chess.BLACK and board.is_queenside_castling(move):
					positions.append(('♜', 'a8'))
					positions.append(('♜', 'd8'))

			if board.is_capture(move):
				capture = True

				if board.is_en_passant(move):
					en_passant = True

			san = board.san(move)

			if '+' in san:
				check = True

			if '#' in san:
				check = True
				checkmate = True

		tokens = np.zeros((self.nb_layers, len(positions) + 1), dtype = np.uint8)

		for i in range(len(positions)):

			tokens[0, i] = self.to_position_ids[positions[i][1]]
			tokens[1, i] = self.to_piece_ids[positions[i][0]]

			if i == 1:
				tokens[2, i] = 1 if capture else 0
				tokens[3, i] = 1 if en_passant else 0
				tokens[4, i] = 1 if check else 0
				tokens[5, i] = 1 if checkmate else 0

		tokens[0, -1] = END_MOVE_TOKEN

		return tokens


	def encode_game(self, game: dict[str, str | None | int | list[str]]) -> npt.NDArray[np.uint8]:

		start = self.encode_game_start(game['winner'])
		moves: list[npt.NDArray[np.uint8]] = []

		for move in game['moves_custom']:
			moves.append(self.encode_move(move))

		end = np.zeros((self.nb_layers, 1), dtype = np.uint8)
		end[0, 0] = END_GAME_TOKEN

		tokens = np.concatenate([start] + moves + [end], axis = 1)

		return tokens


	def encode_game_start(self, winning_color: str | None) -> npt.NDArray[np.uint8]:

		tokens = np.zeros((self.nb_layers, 4), dtype = np.uint8)

		tokens[0, 0] = START_GAME_TOKEN
		tokens[0, 1] = START_INFO_TOKEN
		tokens[0, 2] = self.to_position_ids[winning_color] if winning_color is not None else self.to_position_ids['draw']
		tokens[0, 3] = END_INFO_TOKEN

		return tokens
