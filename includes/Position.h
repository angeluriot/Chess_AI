#ifndef POSITION_HPP
#define POSITION_HPP

#include "utils.h"

struct Position
{
	int8_t x, y;

	static Position invalid;

	Position() : x(0), y(0) {}
	Position(int8_t x, int8_t y) : x(x), y(y) {}

	void operator=(std::array<int8_t, 2> arr) { x = arr[0]; y = arr[1]; }
	Position operator+(const Position& pos) const { return Position(pos.x + x, pos.y + y); }
	bool operator==(const Position& pos) const { return (x == pos.x && y == pos.y); }
	bool operator!=(const Position& pos) const { return !(*this == pos); }

	bool is_valid() const
	{
		return !(x < 0 || x > 7 || y > 7 || y < 0);
	}
};

#endif
