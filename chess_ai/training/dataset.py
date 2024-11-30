import os, random
import numpy as np
import torch
from torch.utils.data import IterableDataset, DataLoader

from chess_ai.data.tokenizer import Tokenizer
from chess_ai.settings import *


class Dataset(IterableDataset):

	def __init__(self, path: str, nb_layers: int) -> None:

		self.data = np.memmap(path, dtype = np.uint8, mode = 'r')
		self.size = self.data.shape[0] // nb_layers
		self.data = np.memmap(path, dtype = np.uint8, shape = (nb_layers, self.size), mode = 'r')


	def __iter__(self):

		while True:

			i = random.randint(0, self.size - 2 - MAX_CONTEXT)
			data = self.data[:, i:i + MAX_CONTEXT + 1]

			x = torch.tensor(data[:, :-1].astype(np.int64), dtype = torch.long, device = 'cpu')
			y = torch.tensor(data[:, 1:].astype(np.int64), dtype = torch.long, device = 'cpu')
			strength = torch.ones((MAX_CONTEXT,), dtype = torch.float32, device = 'cpu')

			yield x, y, strength


def get_loaders(tokenizer: Tokenizer) -> tuple[DataLoader, DataLoader, int, int]:

	train_dataset = Dataset(os.path.join(DATA_DIR, 'train.bin'), tokenizer.nb_layers)
	val_dataset = Dataset(os.path.join(DATA_DIR, 'val.bin'), tokenizer.nb_layers)

	train_loader = DataLoader(
			dataset = train_dataset,
			batch_size = BATCH_SIZE,
			shuffle = False,
			num_workers = 0
		)

	val_loader = DataLoader(
			dataset = val_dataset,
			batch_size = BATCH_SIZE,
			shuffle = False,
			num_workers = 0
		)

	return iter(train_loader), iter(val_loader), train_dataset.size, val_dataset.size
