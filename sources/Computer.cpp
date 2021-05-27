#include "Computer.h"
#include <chrono>
#include <future>
#include <tuple>
#include <random>

Computer::Computer()
{
	transposition_table.resize(67108864, PositionSave{0, -1, 0, true, NodeType::EXACT, Move::no_move});
}

std::pair<Move, int> Computer::find_move(Board board, uint8_t depth, int alpha, int beta, bool maximize)
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
	if (depth == 0 || board.is_finished())
		return std::make_pair(Move::no_move, (maximize ? 1 : -1) * board.get_score());

	int best_score = std::numeric_limits<int>::min() + 1;
	Move best_move = Move::no_move;
	auto move_list = board.generate_moves(board.player_turn);

	move_list.sort([this, board](Move& a, Move& b) {
		Type at_a = board.board[a.target.x][a.target.y], at_b = board.board[b.target.x][b.target.y];

		int a_score = 0;
		int b_score = 0;

		// Capture
		if (at_a != Type::No_Piece)
			a_score += std::abs(get_value(at_a)) - (std::abs(get_value(board.board[a.start.x][a.start.y])) / 10);
		if (at_b != Type::No_Piece)
			b_score += std::abs(get_value(at_b)) - (std::abs(get_value(board.board[b.start.x][b.start.y])) / 10);

		if (a.target == board.last_move.target)
			a_score += 1001;
		if (b.target == board.last_move.target)
			b_score += 1001;

		return a_score >= b_score;
	});

	for (auto& move : move_list)
	{
		int score = -find_move(board.get_moved_board(move), depth - 1, -beta, -alpha, !maximize).second;
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
	static OpeningBook opening_book("./Perfect_2021/BIN/Perfect2021.bin");

	auto book_move = opening_book.book_move(board);
	if (board.half_turn < 1000 && book_move.first != Move::no_move)
		board.move_piece(book_move.first);
	else
	{
		for (auto& tt_pos : transposition_table)
			tt_pos.ancient = true;
		board.move_piece(find_move(board, depth, -1000000000, 1000000000, board.player_turn == Color::White).first);
	}

	board.player_turn = Color(board.player_turn * -1);
}
