#ifndef MOVE_HPP
#define MOVE_HPP

#include "utils.h"

struct Move
{
	Position start, target;

	static Move no_move;

	Move();
	Move(Position start, Position target);
};

Move Move::no_move = Move({ -1, -1 }, { -1, -1 });

#endif
