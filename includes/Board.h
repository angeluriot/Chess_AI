#ifndef BOARD_H
#define BOARD_H

#include "utils.h"

class Board
{
	std::array<std::array<Piece*, 8>, 8> board;
	std::list<Piece> pieces;

	uint16_t get_white_score();
	uint16_t get_black_score();
	void operator=(const Board& board);
};

#endif