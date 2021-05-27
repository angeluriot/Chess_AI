#include "Piece.h"

Type get_type_from_fen(char c)
{
	char c2 = std::toupper(c);
	Type ret = Type::No_Piece;

	switch (c2)
	{
		case 'R':
			ret = Type::White_Rook; break;
		case 'N':
			ret = Type::White_Knight; break;
		case 'B':
			ret = Type::White_Bishop; break;
		case 'Q':
			ret = Type::White_Queen; break;
		case 'K':
			ret = Type::White_King; break;
		case 'P':
			ret = Type::White_Pawn; break;
	}
	return Type(ret * (std::isupper(c) ? 1 : -1));
}

Color get_color(Type piece)
{
	if (piece == Type::Out_Of_Range || piece == Type::No_Piece)
		return Color::Empty;
	return (piece > 0 ? Color::White : Color::Black);
}

int get_value(Type piece)
{
	switch (std::abs(piece))
	{
	case Type::White_Pawn:
		return 100 * get_color(piece);
	case Type::White_Knight:
		return 300 * get_color(piece);
	case Type::White_Bishop:
		return 300 * get_color(piece);
	case Type::White_Rook:
		return 500 * get_color(piece);
	case Type::White_Queen:
		return 900 * get_color(piece);
	case Type::White_King:
		return 90000 * get_color(piece);
	default:
		return 0;
	}
}

const std::list<Position>& get_offsets(Type type)
{
	static std::list<Position> white_pawn_off = { {0, -1}, {0, -2}, {1, -1}, {-1, -1} };
	static std::list<Position> black_pawn_off = { {0, 1}, {0, 2}, {1, 1}, {-1, 1} };
	static std::list<Position> knight_off = { {2, 1}, {2, -1}, {-2, -1}, {-2, 1}, {1, 2}, {-1, 2}, {-1, -2}, {1, -2} };
	static std::list<Position> bishop_off = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
	static std::list<Position> rook_off = { {0, 1}, {0, -1}, {-1, 0}, {1, 0} };
	static std::list<Position> queen_off = { {0, 1}, {0, -1}, {-1, 0}, {1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
	static std::list<Position> king_off = { {0, 1}, {0, -1}, {-1, 0}, {1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

	int8_t abs = std::abs(type);
	switch (abs)
	{
		case Type::White_Pawn:
			return type == Type::Black_Pawn ? black_pawn_off : white_pawn_off;
		case Type::White_Knight:
			return knight_off;
		case Type::White_Bishop:
			return bishop_off;
		case Type::White_Rook:
			return rook_off;
		case Type::White_Queen:
			return queen_off;
		case Type::White_King:
				return king_off;
		default:
			return white_pawn_off;
	}
	return white_pawn_off;
}

bool is_linear(Type type)
{
	return (std::abs(type) == Type::White_Rook || std::abs(type) == Type::White_Bishop || std::abs(type) == Type::White_Queen);
}
