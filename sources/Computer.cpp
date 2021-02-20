#include "Computer.h"

Computer::Computer(PieceColor color)
{
	this->color = color;
}

Move Computer::find_move(uint8_t depth, PieceColor color, const Move& move)
{
	if (move != Move::no_move)
		virtual_board.move_piece(move);

	int16_t score = 0;
	int16_t max_score = -3000;
	Move best_move;

	virtual_board.update_moves(color);

	if (depth == 1)
	{
		for (auto& piece : virtual_board.pieces)
			for (auto& m : piece.moves)
			{
				score = virtual_board.move_score(m);

				if (max_score < score)
				{
					max_score = score;
					best_move = m;
				}
			}

		return best_move;
	}

	for (auto& piece : virtual_board.pieces)
		for (auto& m : piece.moves)
		{
			score = virtual_board.move_score(find_move(depth - 1, (color == White ? Black : White), m));

			if (max_score < score)
			{
				max_score = score;
				best_move = m;
			}
		}
}

void Computer::move(Board& board, uint8_t depth)
{
	virtual_board = board;

	board.move_piece(find_move(depth, color));
}