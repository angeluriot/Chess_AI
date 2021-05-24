#include "Computer.h"
#include <chrono>
#include <future>
#include <tuple>

Computer::Computer()
{
	for (int i = 0; i < zobrist_keys.size(); i++)
		zobrist_keys[i] = rand();
	transposition_table.resize(64000031, PositionSave{0, -1, 0, true, NodeType::EXACT, Move::no_move});
}

uint64_t Computer::get_key(uint8_t x, uint8_t y, Type type) const
{
	switch (type)
	{
		case Type::Black_Bishop:
			return zobrist_keys[64 * 0 + (x - 2) * 8 + (y - 2)];
		case Type::Black_Rook:
			return zobrist_keys[64 * 1 + (x - 2) * 8 + (y - 2)];
		case Type::Black_Knight:
			return zobrist_keys[64 * 2 + (x - 2) * 8 + (y - 2)];
		case Type::Black_Queen:
			return zobrist_keys[64 * 3 + (x - 2) * 8 + (y - 2)];
		case Type::Black_King:
			return zobrist_keys[64 * 4 + (x - 2) * 8 + (y - 2)];
		case Type::Black_Pawn:
			return zobrist_keys[64 * 5 + (x - 2) * 8 + (y - 2)];
		case Type::White_King:
			return zobrist_keys[64 * 6 + (x - 2) * 8 + (y - 2)];
		case Type::White_Rook:
			return zobrist_keys[64 * 7 + (x - 2) * 8 + (y - 2)];
		case Type::White_Knight:
			return zobrist_keys[64 * 8 + (x - 2) * 8 + (y - 2)];
		case Type::White_Queen:
			return zobrist_keys[64 * 9 + (x - 2) * 8 + (y - 2)];
		case Type::White_Bishop:
			return zobrist_keys[64 * 10 + (x - 2) * 8 + (y - 2)];
		case Type::White_Pawn:
			return zobrist_keys[64 * 11 + (x - 2) * 8 + (y - 2)];
		default:
			return 0;
	}
	return 0;
}

uint64_t Computer::signature_hash(const Board& board) const
{
	uint64_t ret = 0;
	bool first = true;
	for (int x = 2; x < 10; x++)
	{
		for (int y = 2; y < 10; y++)
		{
			if (board.board[x][y] == Type::No_Piece)
				continue;
			ret ^= get_key(x, y, board.board[x][y]);
		}
	}
	if (board.player_turn == Color::Black)
		ret ^= zobrist_keys[64 * 12];
	if (board.en_passant != Position::invalid)
		ret ^= zobrist_keys[64 * 12 + 1 + (board.en_passant.x - 2)];

	uint8_t offset = 0;
	for (auto& pair : board.allowed_castle)
	{
		for (bool allowed : pair.second)
		{
			ret ^= zobrist_keys[64 * 12 + 1 + 8 + offset];
			offset++;
		}
	}
	return ret;
}

std::pair<Move, float> Computer::find_move(Board board, uint8_t depth, float alpha, float beta, bool maximize)
{
	uint64_t hash = signature_hash(board);
	auto& tt_pos = transposition_table[hash % transposition_table.size()];
	float alpha_original = alpha;

	if (tt_pos.depth >= depth && tt_pos.hash == hash)
	{
		tt_pos.ancient = false;
		switch (tt_pos.type)
		{
			case NodeType::EXACT:
				return std::make_pair(tt_pos.best_move, tt_pos.score);
			case NodeType::UPPERBOUND:
				alpha = std::max(alpha, tt_pos.score);
			case NodeType::LOWERBOUND:
				beta = std::min(beta, tt_pos.score);
		}

		if (alpha >= beta)
			return std::make_pair(tt_pos.best_move, tt_pos.score);
	}
	if (depth == 0 || board.is_finished())
		return std::make_pair(Move::no_move, (maximize ? 1 : -1) * board.get_score(board.player_turn));

	float best_score = -std::numeric_limits<float>::infinity();
	Move best_move = Move::no_move;
	auto move_list = board.generate_moves(board.player_turn);
	for (auto& move : move_list)
	{
		float score = -find_move(board.get_moved_board(move), depth - 1, -beta, -alpha, !maximize).second;
		if (best_score < score)
		{
			best_score = score;
			best_move = move;
		}
		alpha = std::max(alpha, best_score);
		if (alpha >= beta)
			break;
	}

	if (depth >= tt_pos.depth || tt_pos.ancient)
	{
		if (best_score <= alpha_original)
			transposition_table[hash % transposition_table.size()] = PositionSave{hash, static_cast<int8_t>(depth), best_score, false, NodeType::UPPERBOUND, best_move};
		else if (best_score >= beta)
			transposition_table[hash % transposition_table.size()] = PositionSave{hash, static_cast<int8_t>(depth), best_score, false, NodeType::LOWERBOUND, best_move};
		else
			transposition_table[hash % transposition_table.size()] = PositionSave{hash, static_cast<int8_t>(depth), best_score, false, NodeType::EXACT, best_move};
	}
	return std::make_pair(best_move, best_score);
}

void Computer::move(Board& board, uint8_t depth)
{
	for (auto& tt_pos : transposition_table)
		tt_pos.ancient = true;

	board.move_piece(find_move(board, depth, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), board.player_turn == Color::White).first);
	board.player_turn = Color(board.player_turn * -1);
}
