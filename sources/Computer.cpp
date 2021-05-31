#include "Computer.h"
#include "Piece.h"
#include <chrono>
#include <future>
#include <tuple>
#include <random>
#include "OpeningBook.h"

Computer::Computer()
{
	transposition_table.resize(67108864, PositionSave{0, -1, 0, true, NodeType::EXACT, No_Square});
}

std::pair<uint16_t, int> Computer::find_move(const BitBoardGlobals& globals, BitBoard board, uint8_t depth, int alpha, int beta, bool maximize)
{
	uint64_t hash = board.signature_hash(globals);

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
	if (depth == 0 || board.is_finished())
		return std::make_pair(No_Square, (maximize ? 1 : -1) * board.get_score(globals));

	int best_score = std::numeric_limits<int>::min() + 1;
	uint16_t best_move = No_Square;
	auto move_list = board.generate_moves(globals);

	std::sort(move_list.begin(), move_list.end(), [board](uint16_t a, uint16_t b) {
		uint8_t a_start = ((a & 0xFF00) >> 8), a_target = a & 0xFF;
		uint8_t b_start = ((b & 0xFF00) >> 8), b_target = b & 0xFF;

		int a_score = 0;
		int b_score = 0;

		uint8_t piece_at_a_target = board.piece_at(a_target);
		uint8_t piece_at_b_target = board.piece_at(b_target);
		//uint8_t piece_at_a_start = board.piece_at(a_start);
		//uint8_t piece_at_b_start = board.piece_at(b_start);


		// Capture
		if (a_target != No_Piece)
			a_score += std::abs(get_value(piece_at_a_target)) /*- (std::abs(get_value(piece_at_a_start)) / 10)*/;
		if (b_target != No_Piece)
			b_score += std::abs(get_value(piece_at_b_target)) /*- (std::abs(get_value(piece_at_b_start)) / 10)*/;

		if (a_target == board.last_move & 0xFF)
			a_score += 1001;
		if (b_target == board.last_move & 0xFF)
			b_score += 1001;

		return a_score > b_score;
	});

	for (auto& move : move_list)
	{
		int score = -find_move(globals, board.get_moved_board(move), depth - 1, -beta, -alpha, !maximize).second;
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

void Computer::move(const BitBoardGlobals& globals, BitBoard& board, uint8_t depth)
{
	static OpeningBook opening_book("./Perfect_2021/BIN/Perfect2021.bin");

	auto book_move = opening_book.book_move(globals, board);
	if (board.half_turn < 1000 && book_move.first != No_Square)
		board.move_piece(book_move.first);
	else
	{
		for (auto& tt_pos : transposition_table)
			tt_pos.ancient = true;
		board.move_piece(find_move(globals, board, depth, -1000000000, 1000000000, board.side_to_move == Color::White).first);
	}
}
