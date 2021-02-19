#ifndef PIECE_TYPE_HPP
#define PIECE_TYPE_HPP

#include "Position.h"
#include "utils.h"

struct PieceType
{
	enum Type
	{
		Rook, Bishop, Queen, Knight, King, Pawn
	};

	Type type;
	std::list<Position> offsets;
	bool is_linear;
	uint16_t value;

	static PieceType rook;
	static PieceType bishop;
	static PieceType queen;
	static PieceType knight;
	static PieceType king;
	static PieceType white_pawn;
	static PieceType black_pawn;

	PieceType(Type type, std::list<Position> offsets, bool is_linear, uint16_t value)
		: type(type), offsets(offsets), is_linear(is_linear), value(value) {}
};

#endif
