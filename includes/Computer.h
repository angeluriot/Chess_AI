#ifndef COMPUTER_H
#define COMPUTER_H

#include "utils.h"
#include "Board.h"

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
	Move best_move;
};

class Computer
{
public:

	std::array<uint64_t, 781> zobrist_keys;
	std::vector<PositionSave> transposition_table;

	Computer();

	std::pair<Move, int> find_move(Board board, uint8_t depth, int alpha, int beta, bool maximize);
	void move(Board& board, uint8_t depth);
	uint64_t signature_hash(const Board& board) const;
	uint64_t get_key(uint8_t x, uint8_t y, Type type) const;
};

#endif
