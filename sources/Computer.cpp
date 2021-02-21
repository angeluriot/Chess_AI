#include "Computer.h"

Computer::Computer(Color color)
{
	this->color = color;
}

std::pair<Move, float> Computer::find_move(Board board, uint8_t depth, Color color)
{
	float score = 0.;
	float max_score = -3000.;
	Move best_move;

	board.generate_moves(color);

	if (depth == 1)
	{
		for (auto& move : (color == White ? board.white_moves : board.black_moves))
		{
			score = board.move_score(move, color) + random_float(0.001, 0.005);

			if (max_score < score)
			{
				max_score = score;
				best_move = move;
			}
		}

		return std::make_pair(best_move, max_score);
	}

	for (auto& move : (color == White ? board.white_moves : board.black_moves))
	{
		score = board.move_score(move, color) - find_move(board.get_moved_board(move), depth - 1, (color == White ? Black : White)).second + random_float(0.001, 0.005);

		if (max_score < score)
		{
			max_score = score;
			best_move = move;
		}
	}

	return std::make_pair(best_move, max_score);
}

void Computer::move(Board& board, uint8_t depth)
{
	board.move_piece(find_move(board, depth, color).first);
	board.player_turn = Color(board.player_turn * -1);
	board.generate_moves(board.player_turn);
}
