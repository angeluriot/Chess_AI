#ifndef COMPUTER_H
#define COMPUTER_H

#include "utils.h"
#include "BitBoard.h"
#include <chrono>

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

	std::pair<uint16_t, int> find_move(BitBoard board, uint8_t depth, int alpha, int beta, bool maximize, const std::chrono::high_resolution_clock::time_point& begin);
	bool compare_moves(BitBoard& board, uint16_t a, uint16_t b);
	void sort_moves(BitBoard& board, std::vector<uint16_t>& moves, uint16_t max);
	void move(BitBoard& board);
};

#endif
