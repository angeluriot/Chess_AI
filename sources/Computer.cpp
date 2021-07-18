#include "Computer.h"
#include "Piece.h"
#include <chrono>
#include <future>
#include <tuple>
#include <random>
#include "OpeningBook.h"

bool Computer::compare_moves(BitBoard& board, uint16_t a, uint16_t b)
{
	uint64_t a_hash = board.get_moved_board(a).signature_hash();
	uint64_t b_hash = board.get_moved_board(b).signature_hash();

	bool a_in_tt = transposition_table[a_hash % transposition_table.size()].depth != -1 && transposition_table[a_hash % transposition_table.size()].hash == a_hash;
	bool b_in_tt = transposition_table[b_hash % transposition_table.size()].depth != -1 && transposition_table[b_hash % transposition_table.size()].hash == b_hash;
	if (a_in_tt != b_in_tt)
		return transposition_table[a_hash % transposition_table.size()].depth >= transposition_table[b_hash % transposition_table.size()].depth;

	uint8_t a_start = (a & 0xFF00) >> 8, b_start = (b & 0xFF00) >> 8, a_target = (a & 0xFF), b_target = (b & 0xFF);

	uint8_t piece_at_a_target = board.piece_at(a_target), piece_at_b_target = board.piece_at(b_target);
	uint8_t piece_at_a_start = board.piece_at(a_start), piece_at_b_start = board.piece_at(b_start);

	int a_capture_score = std::abs(get_value(piece_at_a_target)) - std::abs(get_value(piece_at_a_start));
	int b_capture_score = std::abs(get_value(piece_at_b_target)) - std::abs(get_value(piece_at_b_start));
	if (a_capture_score != b_capture_score)
		return a_capture_score > b_capture_score;

	return true;
}

void Computer::sort_moves(BitBoard& board, std::vector<uint16_t>& moves, uint16_t max)
{
	int min_idx = -1;

    for (auto i = 0; i < max && i < moves.size(); i++)
    {
		min_idx = i;
		for (auto j = i + 1; j < moves.size(); j++)
			if (compare_moves(board, moves[j], moves[min_idx]))
				min_idx = j;

		int tmp = moves[min_idx];
		moves[min_idx] = moves[i];
		moves[i] = tmp;
	}
}

Computer::Computer()
{
	transposition_table.resize(128000000, PositionSave{0, -1, 0, true, NodeType::EXACT, No_Square});
}

std::pair<uint16_t, int> Computer::find_move(BitBoard board, uint8_t depth, int alpha, int beta, bool maximize, const std::chrono::high_resolution_clock::time_point& begin)
{
	uint64_t hash = board.signature_hash();

	auto& tt_pos = transposition_table[hash % transposition_table.size()];
	int alpha_original = alpha;

	if (tt_pos.depth >= depth && tt_pos.hash == hash)
	{
		tt_pos.ancient = false;
		switch (tt_pos.type)
		{
			case NodeType::EXACT:
				return std::make_pair(tt_pos.best_move, tt_pos.score);
			case NodeType::UPPERBOUND:
				alpha = std::max(alpha, tt_pos.score);
				break;
			case NodeType::LOWERBOUND:
				beta = std::min(beta, tt_pos.score);
				break;
		}

		if (alpha >= beta)
			return std::make_pair(tt_pos.best_move, tt_pos.score);
	}
	if (board.is_finished())
		return std::make_pair(No_Square, (maximize ? 1 : -1) * (board.piece_boards[Piece::White_King] == 0 ? -999999 : 999999));
	if (depth == 0)
		return std::make_pair(No_Square, (maximize ? 1 : -1) * board.get_score());

	int best_score = std::numeric_limits<int>::min() + 1;
	uint16_t best_move = No_Square;
	auto move_list = board.generate_moves();

	std::sort(move_list.begin(), move_list.end(), [board](uint16_t a, uint16_t b) {
		uint8_t a_start = ((a & 0xFF00) >> 8), a_target = a & 0xFF;
		uint8_t b_start = ((b & 0xFF00) >> 8), b_target = b & 0xFF;

		int a_score = std::abs(get_value(board.piece_at(a_target))) - std::abs(get_value(board.piece_at(a_start)));
		int b_score = std::abs(get_value(board.piece_at(b_target))) - std::abs(get_value(board.piece_at(b_start)));

		return a_score > b_score;
	});

	for (auto& move : move_list)
	{
		if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - begin).count() >= 8)
			return std::make_pair(No_Square, -12345678);
		int score = -find_move(board.get_moved_board(move), depth - 1, -beta, -alpha, !maximize, begin).second;
		if (score == -12345678)
			return std::make_pair(best_move, best_score);
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

void Computer::move(BitBoard& board)
{
	static OpeningBook opening_book("./Perfect_2021/BIN/Perfect2021.bin");


	auto book_move = opening_book.book_move(board);
	if (board.half_turn < 1000 && book_move.first != No_Square)
		board.move_piece(book_move.first);
	else
	{
		std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
		uint8_t depth = 1;
		uint16_t best_move = No_Square;
		while (depth < 8)
		{
			for (auto& tt_pos : transposition_table)
				tt_pos.ancient = true;
			auto ret = find_move(board, depth, std::numeric_limits<int>::min() + 1, std::numeric_limits<int>::max(), board.side_to_move == Color::White, begin);
			if (ret.second != -12345678)
				best_move = ret.first;
			else
				break;
			depth++;
		}
		if (best_move != No_Square)
			board.move_piece(best_move);
	}
}
