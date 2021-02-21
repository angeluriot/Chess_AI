#ifndef PIECE_HPP
#define PIECE_HPP

#include "PieceType.h"
#include "Move.h"
#include "utils.h"
#include "Position.h"

class Board;

enum Color : int8_t
{
	White = 1,
	Black = -1,
	Empty = 0
};

class Piece
{
	private:
		std::list<Move>& generatePawnMoves();

	public:
		PieceType* type;
		Color color;
		Color enemy_color;
		Position pos;
		Board* board;
		std::list<Move> moves;

		Piece(PieceType* type, Color color, Position pos, Board* board);
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
