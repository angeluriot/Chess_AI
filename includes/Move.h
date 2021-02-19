#ifndef MOVE_HPP
#define MOVE_HPP

#include "utils.h"

struct Move
{
	Position start, target;

	static Move no_move;

	Move() : start({0, 0}), target({0, 0}) {}
	Move(Position start, Position target) : start(start), target(target) {}
};

Move Move::no_move = Move({ -1, -1 }, { -1, -1 });

#endif
