#ifndef MOVE_HPP
#define MOVE_HPP

#include "utils.h"
#include "Position.h"

class Board;

struct Move
{
	Position start, target;
	Board* board;

	static Move no_move;

	Move() : start({0, 0}), target({0, 0}), board(NULL) {}
	Move(Position start, Position target, Board* board) : start(start), target(target), board(board) {}

	void makeMove() const;
	void unmakeMove() const;
};

#endif
