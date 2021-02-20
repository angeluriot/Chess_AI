#include "Computer.h"

Computer::Computer(PieceColor color)
{
	this->color = color;
}

Move Computer::find_move(uint8_t depth, PieceColor color, const Move& move)
{
	if (move != Move::no_move)
		virtual_board.move_piece(move);

	int16_t max_score = -3000;
	Move best_move;

	if (depth == 1)
	{
		virtual_board.update_moves(color);

		for (auto& piece : virtual_board.pieces)
			for (auto& m : piece.moves)
				if (max_score < virtual_board.move_score(m))
				{
					max_score = virtual_board.move_score(m);
					best_move = m;
				}

		return best_move;
	}

	return Move::no_move;
}

void Computer::move(Board& board, uint8_t depth)
{
	virtual_board = board;

	board.move_piece(find_move(depth, color));
}