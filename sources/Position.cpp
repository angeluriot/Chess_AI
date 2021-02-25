#include "Position.h"

Position Position::invalid = Position(-1, -1);

std::ostream& operator<<(std::ostream& os, const Position& pos)
{
	os << (int)pos.x << " " << (int)pos.y << std::endl;
	return os;
}
