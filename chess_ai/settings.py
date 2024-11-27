import torch
from contextlib import nullcontext

# ============== Dataset ============== #

DATA_DIR = 'data'
OUTPUT_DIR = 'output'
NUM_THREADS = 16
VAL_RATIO = 0.01

CONTROLS = ['<unk>', '<pad>', '<g>', '<g/>', '<i>', '<i/>', '<m/>']
UNKNOWN_TOKEN = 0
PADDING_TOKEN = 1
START_GAME_TOKEN = 2
END_GAME_TOKEN = 3
START_INFO_TOKEN = 4
END_INFO_TOKEN = 5
END_MOVE_TOKEN = 6

# =============== Model =============== #

MAX_CONTEXT = 1024
EMBEDDING_DIM = 512
NUM_GROUPED_HEADS = 4
NUM_HEADS = 8
HEAD_DIM = EMBEDDING_DIM // NUM_HEADS
FFN_DIM = int((2.0 / 3.0) * 4 * EMBEDDING_DIM)
NUM_BLOCKS = 8
DROPOUT = 0
INIT_STDDEV = 0.02
ROPE_THETA = 10_000.0

# ============= Training ============== #

BATCH_SIZE = 32
NUM_ACCUMULATIONS = 16

MAX_LEARNING_RATE = 1e-3
MIN_LEARNING_RATE = 1e-4
WARMUP_STEPS = 2_000
DECAY_STEPS = 8_000
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

STOCKFISH_PATH = 'E:/Angel/Development/Others/stockfish_16/stockfish-windows-x86-64-avx2.exe'
NB_RANDOM_MOVES = 3
NB_EVAL_GAMES = 128

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
