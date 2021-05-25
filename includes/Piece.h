#ifndef PIECE_HPP
#define PIECE_HPP

#include "Move.h"
#include "utils.h"
#include "Position.h"

class Board;

enum Color : int
{
	White = 1,
	Black = -1,
	Empty = 0
};

enum Type : int
{
	No_Piece = 0,
	Out_Of_Range = 99,

	White_Pawn = 1,
	White_Knight = 2,
	White_Bishop = 3,
	White_Rook = 4,
	White_Queen = 5,
	White_King = 6,

	Black_Pawn = -1,
	Black_Knight = -2,
	Black_Bishop = -3,
	Black_Rook = -4,
	Black_Queen = -5,
	Black_King = -6,
};

Type get_type_from_fen(char c);
Color get_color(Type type);
int get_value(Type piece);
const std::list<Position>& get_offsets(Type type);
bool is_linear(Type type);

#endif
