from chess_ai.data.tokenizers.uci import UCITokenizer
from chess_ai.settings import *
from chess import Move


class Tokenizer:

	def __init__(self) -> None:

		self.engine = UCITokenizer()


	@property
	def vocab(self) -> list[str]:

		return self.engine.vocab


	@property
	def to_id(self) -> dict[str, int]:

		return self.engine.to_id


	@property
	def to_token(self) -> dict[int, str]:

		return self.engine.to_token


	@property
	def vocab_size(self) -> int:

		return self.engine.vocab_size


	def encode_move(self, move: Move | str) -> list[int]:

		return self.engine.encode_move(move)


	def encode_game(self, game: dict[str, str | None | int | list[str]]) -> list[int]:

		return self.engine.encode_game(game)


	def encode_game_start(self, winning_color: str | None) -> list[int]:

		return self.engine.encode_game_start(winning_color)
