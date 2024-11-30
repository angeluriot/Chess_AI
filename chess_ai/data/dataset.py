import os, random
from tqdm import tqdm
import numpy as np
import numpy.typing as npt
from chess_ai.settings import *
from chess_ai.data.tokenizer import Tokenizer
from datasets import load_dataset


class Dataset:

	def __init__(self, tokenizer: Tokenizer) -> None:

		self.tokenizer = tokenizer

		self.dataset = load_dataset(
			path = 'angeluriot/chess_games',
			split = 'train'
		)

		self.nb_games = len(self.dataset)

		os.makedirs(DATA_DIR, exist_ok = True)


	def get_game(self, i: int | None = None) -> dict[str, str | None | int | list[str]]:

		if i is None:
			i = random.randint(0, self.nb_games - 1)

		return self.dataset[i]


	def game_to_tokens(self, game: dict[str, str | None | int | list[str]]) -> dict[str, npt.NDArray[np.uint8] | int]:

		tokens = self.tokenizer.encode_game(game)

		return {'tokens': tokens, 'size': tokens.shape[1]}


	def save(self):

		if os.path.exists(os.path.join(DATA_DIR, f'train.bin')):
			return

		os.makedirs(os.path.join(DATA_DIR), exist_ok = True)

		self.dataset = self.dataset.train_test_split(test_size = VAL_RATIO, shuffle = True)
		self.dataset['val'] = self.dataset.pop('test')

		tokenized = self.dataset.map(
			lambda game: self.game_to_tokens(game),
			desc = 'Tokenizing dataset',
			num_proc = NUM_THREADS
		)

		for split, documents in tokenized.items():

			batch_size = 1_024

			while batch_size >= len(documents):
				batch_size //= 2

			size = int(np.sum(documents['size'], dtype = np.uint64))
			path = os.path.join(DATA_DIR, f'{split}.bin')
			file = np.memmap(path, dtype = np.uint8, mode = 'w+', shape = (self.tokenizer.nb_layers, size))
			i = 0

			for batch_i in tqdm(range(batch_size), desc = f'Saving {split}'):

				batch = documents.shard(num_shards = batch_size, index = batch_i, contiguous = True).with_format('numpy')
				file_batch = np.concatenate(batch['tokens'], axis = 1)
				file[:, i:i + file_batch.shape[1]] = file_batch
				i += file_batch.shape[1]

			file.flush()
