#ifndef COMPUTER_H
#define COMPUTER_H

#include "utils.h"
#include "Board.h"

class Computer
{
public:

	Board virtual_board;
	PieceColor color;

	Computer(PieceColor color);

	Move find_move(const Board& board, uint8_t depth, PieceColor color, const Move& move = Move::no_move);
	void move(Board& board, uint8_t depth);
};

#endif
