from chess_ai.settings import *
from chess import Move


class UCITokenizer:

	def __init__(self) -> None:

		self.vocab = []
		self.vocab += CONTROLS
		self.vocab += ['white', 'black', 'draw']
		self.vocab += [str(l) + str(n) for l in 'abcdefgh' for n in range(1, 9)]
		self.vocab += ['N', 'B', 'R', 'Q', 'n', 'b', 'r', 'q']

		self.to_id = {token: i for i, token in enumerate(self.vocab)}
		self.to_token = {i: token for i, token in enumerate(self.vocab)}

		self.vocab_size = len(self.vocab)


	def encode_move(self, move: Move | str) -> list[int]:

		if isinstance(move, Move):
			move = move.uci()

		tokens = [self.to_id[move[:2]], self.to_id[move[2:4]]]

		if len(move) > 4:
			tokens.append(self.to_id[move[4]])

		tokens.append(END_MOVE_TOKEN)

		return tokens


	def encode_game(self, game: dict[str, str | None | int | list[str]]) -> list[int]:

		tokens = self.encode_game_start(game['winner'])

		for move in game['moves_uci']:
			tokens.extend(self.encode_move(move))

		tokens.append(END_GAME_TOKEN)

		return tokens


	def encode_game_start(self, winning_color: str | None) -> list[int]:

		return [
			START_GAME_TOKEN,
			START_INFO_TOKEN,
			self.to_id[winning_color] if winning_color is not None else self.to_id['draw'],
			END_INFO_TOKEN
		]
