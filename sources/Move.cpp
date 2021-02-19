#include "Move.h"
#include "Board.h"

Move Move::no_move = Move({ -1, -1 }, { -1, -1 }, NULL);

void Move::makeMove() const
{
	(*board)[start]->setPos(target);
}

void Move::unmakeMove() const
{
	(*board)[target]->setPos(start);
}
