import torch, math
from torch import nn
from flash_attn import flash_attn_func

from chess_ai.data.tokenizer import Tokenizer
from chess_ai.training.layers import *
from chess_ai.settings import *
from chess_ai.training.rope import *


class AttentionBlock(Module):

	def __init__(self, **kwargs):

		super().__init__(**kwargs)

		self.query = Linear(EMBEDDING_DIM, NUM_HEADS * HEAD_DIM)
		self.key = Linear(EMBEDDING_DIM, NUM_GROUPED_HEADS * HEAD_DIM)
		self.value = Linear(EMBEDDING_DIM, NUM_GROUPED_HEADS * HEAD_DIM)

		self.projection = Linear(NUM_HEADS * HEAD_DIM, EMBEDDING_DIM)
		nn.init.normal_(self.projection.weight, mean = 0.0, std = INIT_STDDEV / math.sqrt(2 * NUM_BLOCKS))

		self.residual_dropout = nn.Dropout(DROPOUT)


	def forward(self, x: torch.Tensor, rope_frequencies: torch.Tensor) -> torch.Tensor:

		batch_size, context_length, _ = x.shape

		q = self.query(x)
		k = self.key(x)
		v = self.value(x)

		q = q.view(batch_size, context_length, NUM_HEADS, HEAD_DIM)
		k = k.view(batch_size, context_length, NUM_GROUPED_HEADS, HEAD_DIM)
		v = v.view(batch_size, context_length, NUM_GROUPED_HEADS, HEAD_DIM)

		q, k = rotary_position_embedding(q, k, rope_frequencies)

		k = torch.repeat_interleave(k, repeats = NUM_HEADS // NUM_GROUPED_HEADS, dim = 2)
		v = torch.repeat_interleave(v, repeats = NUM_HEADS // NUM_GROUPED_HEADS, dim = 2)

		x = flash_attn_func(q, k, v, dropout_p = DROPOUT if self.training else 0, causal = True)

		x = x.reshape(batch_size, context_length, NUM_HEADS * HEAD_DIM)

		return self.residual_dropout(self.projection(x))


class FeedForward(Module):

	def __init__(self, **kwargs):

		super().__init__(**kwargs)

		self.linear_1 = Linear(EMBEDDING_DIM, FFN_DIM)
		self.linear_2 = Linear(EMBEDDING_DIM, FFN_DIM)
		self.linear_3 = Linear(FFN_DIM, EMBEDDING_DIM)
		self.activation = nn.SiLU()
		self.dropout = nn.Dropout(DROPOUT)


	def forward(self, x: torch.Tensor) -> torch.Tensor:

		x = self.activation(self.linear_1(x)) * self.linear_2(x)
		x = self.dropout(self.linear_3(x))

		return x


# Model block
class TransformerBlock(Module):

	def __init__(self, **kwargs):

		super().__init__(**kwargs)

		self.norm_1 = LayerNorm(EMBEDDING_DIM)
		self.attention = AttentionBlock()
		self.norm_2 = LayerNorm(EMBEDDING_DIM)
		self.feed_forward = FeedForward()


	def forward(self, x: torch.Tensor, rope_frequencies: torch.Tensor) -> torch.Tensor:

		x = x + self.attention(self.norm_1(x), rope_frequencies)
		x = x + self.feed_forward(self.norm_2(x))

		return x


# Model
class Model(Module):

	def __init__(self, tokenizer: Tokenizer, **kwargs):

		super().__init__(**kwargs)

		self.token_embeddings = nn.ModuleList([Embedding(size, EMBEDDING_DIM) for size in tokenizer.layer_sizes])
		self.rope_frequencies = create_rope_frequencies(HEAD_DIM, MAX_CONTEXT)
		self.init_dropout = nn.Dropout(DROPOUT)
		self.blocks = nn.ModuleList([TransformerBlock() for _ in range(NUM_BLOCKS)])
		self.final_norm = LayerNorm(EMBEDDING_DIM)
		self.final_linears = nn.ModuleList([Linear(EMBEDDING_DIM, size) for size in tokenizer.layer_sizes])
		#self.token_embedding.weight = self.final_linear.weight


	def forward(self, input: torch.Tensor) -> list[torch.Tensor]:

		# (batch_size, nb_layers, context_length)
		if input.shape[2] > MAX_CONTEXT:
			input = input[:, :, -MAX_CONTEXT:]

		rope_frequencies = self.rope_frequencies[:input.shape[2]]
		rope_frequencies = rope_frequencies[None, :, None, :]

		# (batch_size, context_length, embedding_dim)
		x = self.token_embeddings[0](input[:, 0, :])

		for i in range(1, len(self.token_embeddings)):
			x += self.token_embeddings[i](input[:, i, :])

		x = self.init_dropout(x)

		for block in self.blocks:
			x = block(x, rope_frequencies)

		x = self.final_norm(x)

		# nb_layers (batch_size, context_length, vocab_size)
		return [linear(x) for linear in self.final_linears]
