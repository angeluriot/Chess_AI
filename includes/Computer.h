#ifndef COMPUTER_H
#define COMPUTER_H

#include "utils.h"
//#include "OpeningBook.h"
#include "BitBoard.h"

enum NodeType : uint8_t
{
	EXACT,
	UPPERBOUND,
	LOWERBOUND
};

struct PositionSave
{
	uint64_t hash;
	int8_t depth;
	int score;
	bool ancient;
	NodeType type;
	uint16_t best_move;
};

class Computer
{
public:
	std::vector<PositionSave> transposition_table;

	Computer();

	std::pair<uint16_t, int> find_move(const BitBoardGlobals& globals, BitBoard board, uint8_t depth, int alpha, int beta, bool maximize);
	void move(const BitBoardGlobals& globals, BitBoard& board, uint8_t depth);
};

#endif
