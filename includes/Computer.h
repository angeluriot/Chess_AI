#ifndef COMPUTER_H
#define COMPUTER_H

#include "utils.h"
#include "Board.h"

class Computer
{
	Board virtual_board;

	Move find_move(uint8_t depth, PieceColor color, Move move = Move::no_move);
};

#endif
