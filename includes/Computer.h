#ifndef COMPUTER_H
#define COMPUTER_H

#include "utils.h"
#include "Board.h"

class Computer
{
public:

	Color color;

	Computer(Color color);
 
	std::pair<Move, float> find_move(const Board& board, uint8_t depth, Color color, const Move& move = Move::no_move);
	void move(Board& board, uint8_t depth);
};

#endif
