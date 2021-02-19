#ifndef POSITION_HPP
#define POSITION_HPP

#include "utils.h"

struct Position
{
	int8_t x, y;

	static Position invalid;

	Position() : x(0), y(0) {}
	Position(int8_t x, int8_t y) : x(x), y(y) {}
	void operator=(std::array<int8_t, 2> arr) { x = arr[0]; y = arr[y]; };
	Position operator+(Position pos) { return Position(pos.x + x, pos.y + y); }

	static bool is_valid(Position pos)
	{
		return !(pos.x < 0 || pos.x > 7 || pos.y > 7 || pos.y < 0);
	}
};

Position Position::invalid = Position(-1, -1);

#endif
