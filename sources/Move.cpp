#include "Move.h"
#include "Board.h"

Move Move::no_move = Move({ -1, -1 }, { -1, -1 });

bool Move::operator==(const Move& other) const
{
	return (start == other.start && target == other.target);
}

bool Move::operator!=(const Move& other) const
{
	return !(*this == other);
} 
