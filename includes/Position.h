#ifndef POSITION_HPP
#define POSITION_HPP

#include "utils.h"

struct Position
{
	uint8_t x, y;

	static Position invalid;

	Position();
	Position(uint8_t x, uint8_t y);
	void operator=(std::array<uint8_t, 2> array);
};

Position Position::invalid = Position(-1, -1);

#endif