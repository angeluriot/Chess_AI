#ifndef COMPUTER_H
#define COMPUTER_H

#include "utils.h"
#include "Board.h"

class Computer
{
public:

	Color color;

	Computer(Color color);

	std::pair<Move, float> find_move(Board board, uint8_t depth, Color color);
	void move(Board& board, uint8_t depth);
};

#endif
