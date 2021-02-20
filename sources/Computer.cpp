#include "Computer.h"

Computer::Computer(PieceColor color)
{
	this->color = color;
}

std::pair<Move, float> Computer::find_move(const Board& board, uint8_t depth, PieceColor color, const Move& move)
{
	Board virtual_board = board;

	if (move != Move::no_move)
		virtual_board.move_piece(move);

	float score = 0.;
	float max_score = -3000.;
	Move best_move;

	virtual_board.generate_moves(color);

	if (depth == 1)
	{
		for (auto& piece : virtual_board.pieces)
		{
			if (piece.color != color)
				continue;

			for (auto& m : piece.moves)
			{
				score = virtual_board.move_score(m, color) + random_float(0.1, 0.5);

				if (max_score < score)
				{
					max_score = score;
					best_move = m;
				}
			}
		}

		return std::make_pair(best_move, max_score);
	}

	for (auto& piece : virtual_board.pieces)
	{
		if (piece.color != color)
			continue;

		for (auto& m : piece.moves)
		{
			score = virtual_board.move_score(m, color) - find_move(virtual_board, depth - 1, (color == White ? Black : White), m).second + random_float(0.1, 0.5);

			if (max_score < score)
			{
				max_score = score;
				best_move = m;
			}
		}
	}

	return std::make_pair(best_move, max_score);
}

void Computer::move(Board& board, uint8_t depth)
{
	board.move_piece(find_move(board, depth, color).first);
}
