#ifndef PIECE_HPP
#define PIECE_HPP

#include "PieceType.h"
#include "Move.h"
#include "utils.h"
#include "Position.h"

enum PieceColor : bool
{
	White,
	Black
};

class Piece
{
	public:
		PieceType& type;
		const PieceColor color;
		Position pos;
		Board* board;
		std::list<Move> moves;

		Piece(PieceType& type, PieceColor color, Position pos, const Board& board);
		Piece(const Piece& other);
		Piece& operator=(const Piece& other);
		virtual ~Piece();

		std::list<Move>& generateMoves();

		void setPos(const Position& position);
		void setPos(int8_t x, int8_t y);
		void move(const Position& offset);
		void move(int8_t x_offset, int8_t y_offset);
		const Position& getPos() const;

};

#endif
