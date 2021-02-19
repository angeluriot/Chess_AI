#ifndef BOARD_H
#define BOARD_H

#include "utils.h"
#include "Piece.h"

class Board
{
	std::array<std::array<Piece*, 8>, 8> board;
	std::list<Piece> pieces;
	Position en_passant;

	Board();
	Board(const Board& other);

	void operator=(const Board& other);
	Piece*& operator[](Position position);

	uint16_t get_score(PieceColor color);
	void update_moves();
	void move_piece(Move move);
};

#endif