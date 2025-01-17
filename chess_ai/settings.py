import torch
from contextlib import nullcontext

# ============== Dataset ============== #

DATA_DIR = 'data'
OUTPUT_DIR = 'output'
NUM_THREADS = 16
VAL_RATIO = 0.01

CONTROLS = ['<pad>', '<unk>', '<g>', '<g/>', '<i>', '<i/>', '<m/>']
PADDING_TOKEN = 0
UNKNOWN_TOKEN = 1
START_GAME_TOKEN = 2
END_GAME_TOKEN = 3
START_INFO_TOKEN = 4
END_INFO_TOKEN = 5
END_MOVE_TOKEN = 6

# =============== Model =============== #

MAX_CONTEXT = 2048
EMBEDDING_DIM = 1024
NUM_GROUPED_HEADS = 4
NUM_HEADS = 16
HEAD_DIM = EMBEDDING_DIM // NUM_HEADS
FFN_DIM = int((2.0 / 3.0) * 4 * EMBEDDING_DIM)
NUM_BLOCKS = 24
DROPOUT = 0
INIT_STDDEV = 0.02
ROPE_THETA = 10_000.0

# ============= Training ============== #

BATCH_SIZE = 4
NUM_ACCUMULATIONS = 64

MAX_LEARNING_RATE = 6e-4
MIN_LEARNING_RATE = 6e-5
WARMUP_STEPS = 2_000
DECAY_STEPS = 28_000
STOP_TRAINING = True

BETA_1 = 0.9
BETA_2 = 0.95
EPSILON = 1e-8
WEIGHT_DECAY = 0.1
CLIP_GRADIENT = 1.0

METRICS_BETA = 0.9
VAL_INTERVAL = 25
ELO_INTERVAL = 1_000

# ============== Testing ============== #

STOCKFISH_PATH = '<path to stockfish>'
NB_RANDOM_MOVES = 3
NB_EVAL_GAMES = 100

# https://github.com/official-stockfish/Stockfish/commit/a08b8d4
STOCKFISH_MIN_ELO = 1_320
STOCKFISH_MAX_ELO = 3_190
ELOS_RANGE = 1_000
CENTERED_LEVEL = 3

# ===================================== #

GPU_ENABLED = torch.cuda.is_available()
FLOAT16_ENABLED = GPU_ENABLED and torch.cuda.is_bf16_supported()
DEVICE_NAME = 'cuda:0' if GPU_ENABLED else 'cpu'
DEVICE = torch.device(DEVICE_NAME)
CONTEXT = torch.autocast(device_type='cuda', dtype=torch.bfloat16) if FLOAT16_ENABLED else nullcontext()
