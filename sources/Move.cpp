#include "Move.h"
#include "Board.h"

Move Move::no_move = Move({ -1, -1 }, { -1, -1 }, NULL);

bool Move::operator==(const Move& other) const
{
	return (start == other.start && target == other.target);
}

bool Move::operator!=(const Move& other) const
{
	return !(*this == other);
}

void Move::makeMove() const
{
	(*board)[start]->setPos(target);
}

void Move::unmakeMove() const
{
	(*board)[target]->setPos(start);
}
