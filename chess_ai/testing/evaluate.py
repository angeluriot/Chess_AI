from chess_ai.data.tokenizer import Tokenizer
from chess_ai.testing.sampling import Sampler
from chess_ai.training.model import Model
from chess_ai.settings import *
from stockfish import Stockfish
from time import time
import random
import numpy as np
from scipy.optimize import minimize


stockfish = Stockfish(
	path = STOCKFISH_PATH,
	parameters = { 'Threads': NUM_THREADS - (NUM_THREADS // 4) }
)


def evaluate(model: Model, tokenizer: Tokenizer, nb_games: int, elo: int, verbose: bool = True) -> tuple[int, int, int]:

	nb_wins = 0
	nb_draws = 0
	nb_losses = 0
	bot_times = []
	opponent_times = []
	nb_moves = 0
	sampler = Sampler(model, tokenizer, 'white')

	stockfish.set_elo_rating(elo)

	for _ in range(nb_games // 2):

		for color in ['white', 'black']:

			sampler.reset(color, NB_RANDOM_MOVES)

			while True:

				start = time()
				stockfish.set_fen_position(sampler.board.fen())
				opponent_move = stockfish.get_best_move()
				opponent_move = sampler.board.parse_uci(opponent_move) if opponent_move is not None else None
				opponent_times.append(time() - start)

				if opponent_move is None or sampler.board.is_game_over():
					break

				nb_moves += 1

				start = time()
				played = sampler.play(opponent_move, verbose = False)
				bot_times.append(time() - start)

				if not played:
					break

				nb_moves += 1

			result = sampler.board.result()
			winner = 'white' if result == '1-0' else 'black' if result == '0-1' else None

			if winner is None:
				nb_draws += 1
			elif winner == color:
				nb_wins += 1
			else:
				nb_losses += 1

			if verbose:
				print(f'Game {nb_wins + nb_draws + nb_losses} / {nb_games}', end = '\r')

	if verbose:

		print(f'Wins: {nb_wins} ({nb_wins / nb_games * 100:.2f}%)')
		print(f'Draws: {nb_draws} ({nb_draws / nb_games * 100:.2f}%)')
		print(f'Losses: {nb_losses} ({nb_losses / nb_games * 100:.2f}%)')
		print(f'Bot mean time: {np.mean(bot_times[1:]):.4f}s')
		print(f'Opponent mean time: {np.mean(opponent_times[1:]):.4f}s')

	return nb_wins, nb_draws, nb_losses


def estimate_elo(model: Model, tokenizer: Tokenizer, previous_elo: int | None = None, verbose: bool = True) -> int:

	center = np.clip(previous_elo, STOCKFISH_MIN_ELO, STOCKFISH_MAX_ELO) if previous_elo is not None else STOCKFISH_MIN_ELO

	elos = np.concatenate([-(np.linspace(0, 1, NB_EVAL_GAMES // 4)[::-1] ** CENTERED_LEVEL), np.linspace(0, 1, NB_EVAL_GAMES // 4) ** CENTERED_LEVEL])
	elos = np.clip(elos * (ELOS_RANGE / 2) + center, STOCKFISH_MIN_ELO, STOCKFISH_MAX_ELO)
	elos = np.round(elos).astype(int).tolist()

	results: list[tuple[int, float]] = []
	nb_wins = 0
	nb_draws = 0
	nb_losses = 0

	for i, elo in enumerate(elos):

		nb_w, nb_d, nb_l = evaluate(model, tokenizer, 2, elo, verbose = False)

		for _ in range(nb_w):
			results.append((elo, 1))
			nb_wins += 1

		for _ in range(nb_d):
			results.append((elo, 0.5))
			nb_draws += 1

		for _ in range(nb_l):
			results.append((elo, 0))
			nb_losses += 1

		if verbose:
			print(f'Game {(i + 1) * 2} / {NB_EVAL_GAMES} ({elo} elo)', end = '\r')

	random.shuffle(results)

	def log_likelihood(elo_model: float, scores: list[tuple[int, float]]) -> float:

		loss = 0

		for elo_stockfish, result in scores:
			proba_model = 1 / (1 + 10 ** ((elo_stockfish - elo_model) / 400))
			loss += result * np.log(proba_model) + (1 - result) * np.log(1 - proba_model)

		return -loss

	elo_model = minimize(log_likelihood, previous_elo if previous_elo is not None else 0, args = (results,), method = 'BFGS').x[0]
	elo_model = max(round(elo_model), 0)

	if verbose:
		print(f'Mean opponent elos: {round(np.array(elos).mean()):,}                        ')
		print(f'Wins: {nb_wins} ({nb_wins / len(results) * 100:.2f}%)')
		print(f'Draws: {nb_draws} ({nb_draws / len(results) * 100:.2f}%)')
		print(f'Losses: {nb_losses} ({nb_losses / len(results) * 100:.2f}%)')
		print(f'Estimated elo: {elo_model:,}')

	return elo_model
