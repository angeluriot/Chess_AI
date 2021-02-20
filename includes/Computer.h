#ifndef COMPUTER_H
#define COMPUTER_H

#include "utils.h"
#include "Board.h"

class Computer
{
	Board virtual_board;

	Move find_move(uint8_t depth, PieceColor color, const Move& move = Move::no_move);
	void move(uint8_t depth, PieceColor color);
};

#endif
