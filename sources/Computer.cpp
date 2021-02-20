#include "Computer.h"

Computer::Computer(PieceColor color)
{
	this->color = color;
}

Move Computer::find_move(const Board& board, uint8_t depth, PieceColor color, const Move& move)
{
	Board board2 = board; 

	if (move != Move::no_move)
		board2.move_piece(move);

	int16_t score = 0;
	int16_t max_score = -3000;
	Move best_move;

	board2.update_moves(color);

	if (depth == 1)
	{
		for (auto& piece : board2.pieces)
		{
			if (piece.color != color)
				continue;
			for (auto& m : piece.moves)
			{
				score = board2.move_score(m);

				if (max_score < score)
				{
					max_score = score;
					best_move = m;
				}
			}
		}
		return best_move;
	}

	for (auto& piece : board2.pieces)
	{
		if (piece.color != color)
			continue;
		for (auto& m : piece.moves)
		{
			score = board2.move_score(m) - board2.move_score(find_move(board, depth - 1, (color == White ? Black : White), m));

			if (max_score < score)
			{
				max_score = score;
				best_move = m;
			}
		}
	}
	board2 = board;
	return best_move;
}

void Computer::move(Board& board, uint8_t depth)
{
	virtual_board = board;

	board.move_piece(find_move(board, depth, color));
}
