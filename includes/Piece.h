#ifndef PIECE_HPP
#define PIECE_HPP

#include "PieceType.h"
#include "Move.h"
#include "utils.h"
#include "Position.h"

class Board;

enum PieceColor : bool
{
	White,
	Black
};

class Piece
{
	private:
		std::list<Move>& generatePawnMoves();

	public:
		PieceType* type;
		PieceColor color;
		PieceColor enemy_color;
		Position pos;
		Board* board;
		std::list<Move> moves;

		Piece(PieceType* type, PieceColor color, Position pos, Board* board);
		Piece(const Piece& other);
		virtual ~Piece();

		Piece& operator=(const Piece& other);
		bool operator==(const Piece& other);

		std::list<Move>& generateMoves();
		bool setPos(const Position& position);
		bool setPos(int8_t x, int8_t y);
		bool move(const Position& offset);
		bool move(int8_t x_offset, int8_t y_offset);
		const Position& getPos() const;


};

#endif
