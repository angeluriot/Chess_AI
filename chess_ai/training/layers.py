import numpy as np
import torch
from torch import nn

from chess_ai.settings import *


class Module(nn.Module):

	def nb_parameters(self) -> int:

		return sum([np.prod(p.size(), dtype = np.int32) for p in self.parameters()])


	def nb_trainable_parameters(self) -> int:

		return sum([np.prod(p.size(), dtype = np.int32) for p in self.parameters() if p.requires_grad])


	def nb_non_trainable_parameters(self) -> int:

		return sum([np.prod(p.size(), dtype = np.int32) for p in self.parameters() if not p.requires_grad])


	def summary(self) -> None:

		print(f'Number of parameters: {self.nb_parameters():,}')
		print(f'Number of trainable parameters: {self.nb_trainable_parameters():,}')
		print(f'Number of non-trainable parameters: {self.nb_non_trainable_parameters():,}')


	def clean_nan(self) -> None:

		for p in self.parameters():
			if p.grad is not None:
				torch.nan_to_num(p.grad, nan = 0, posinf = 1e5, neginf = -1e5, out = p.grad)


	def clip_gradient(self, max_norm: float) -> None:

		nn.utils.clip_grad_norm_(self.parameters(), max_norm)


class Linear(nn.Linear):

	def __init__(self, in_features: int, out_features: int, **kwargs):

		super().__init__(in_features, out_features, False, **kwargs)
		nn.init.normal_(self.weight, mean = 0.0, std = INIT_STDDEV)


class LayerNorm(Module):

	def __init__(self, shape: int, epsilon: float = 1e-5, **kwargs):

		super().__init__(**kwargs)

		self.shape = (shape,)
		self.weight = nn.Parameter(torch.ones(shape))
		self.epsilon = epsilon


	def _normalize(self, x: torch.Tensor):

		return x * torch.rsqrt(x.pow(2).mean(-1, keepdim = True) + self.epsilon)


	def forward(self, x: torch.Tensor):

		return self._normalize(x.float()).type_as(x) * self.weight


class Embedding(nn.Embedding):

	def __init__(self, num_embeddings: int, embedding_dim: int, **kwargs):

		super().__init__(num_embeddings, embedding_dim, padding_idx = PADDING_TOKEN, **kwargs)
		nn.init.normal_(self.weight, mean = 0.0, std = INIT_STDDEV)
		super()._fill_padding_idx_with_zero()
