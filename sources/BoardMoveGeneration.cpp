#include "Board.h"

std::list<Move> Board::generate_moves(Color color)
{
	std::list<Move> ret;

	auto threats = generate_threats(Color(color * -1));

	for (int8_t x = 2; x < 10; x++)
		for (int8_t y = 2; y < 10; y++)
			if (get_color(board[x][y]) == color)
				generate_piece_moves(x, y, color, ret, threats);

	return ret;
}

void Board::generate_pawn_moves(int8_t x, int8_t y, Color color, std::list<Move>& ret)
{
	Color enemy_color = Color(-1 * color);
	Position pos = {x, y};

	auto offsets = get_offsets(at(pos));
	auto offset = offsets.begin();

	if (at(pos + *offset) == Type::No_Piece)
	{
		ret.push_back({ pos, pos + *offset });
		offset++;
		if (at(pos + *offset) == Type::No_Piece && ((color == Color::White && y == 8) || (color == Color::Black && y == 3)))
			ret.push_back({ pos, pos + *offset });
		offset++;
	}
	else
	{
		offset++;
		offset++;
	}

	// Offsets de prise en diagonale
	if (Position(x + offset->x, y + offset->y) == en_passant || get_color(at(pos + *offset)) == enemy_color)
		ret.push_back({ {x, y}, Position(x, y) + *offset });
	offset++;
	if (Position(x + offset->x, y + offset->y) == en_passant || get_color(at(pos + *offset)) == enemy_color)
		ret.push_back({ {x, y}, Position(x, y) + *offset });
}

void Board::generate_piece_moves(int8_t x, int8_t y, Color color, std::list<Move>& ret, const std::list<Move>& threats)
{
	int8_t abs = std::abs(board[x][y]);
	if (abs == Type::White_Pawn)
	{
		generate_pawn_moves(x, y, color, ret);
		return;
	}

	Color enemy_color = Color(color * -1);
	bool linear = is_linear(board[x][y]);

	for (auto& offset : get_offsets(board[x][y]))
	{
		Position actual = {x, y};
		Position pinned = Position::invalid;
		std::list<Move> tmp_moves;
		do
		{
			if (at(actual + offset) == Type::Out_Of_Range || get_color(at(actual + offset)) == color)
				break;
			ret.push_back(Move({x, y}, actual + offset));
			if (get_color(at(actual + offset)) == enemy_color)
				break;
			actual = actual + offset;
		} while (linear);
	}

	if (abs == Type::White_King)
	{
		// Grand roque
		if (allowed_castle.find(color)->second[0] && board[3][y] == Type::No_Piece && board[4][y] == Type::No_Piece && board[5][y] == Type::No_Piece &&
			std::find_if(threats.begin(), threats.end(), [this, x, y](const Move& val) { return (val.target.x == 6 || val.target.x == 4 || val.target.x == 5) && val.target.y == y; }) == threats.end())
			ret.push_back({ {x, y}, Position(x - 2, y) });

		// Petit roque
		if (allowed_castle.find(color)->second[1] && board[7][y] == Type::No_Piece && board[8][y] == Type::No_Piece &&
			std::find_if(threats.begin(), threats.end(), [this, x, y](const Move& val) { return (val.target.x == 6 || val.target.x == 7 || val.target.x == 8) && val.target.y == y; }) == threats.end())
			ret.push_back({ {x, y}, Position(x + 2, y) });
	}
}


std::list<Move> Board::generate_threats(Color color)
{
	std::list<Move> ret;

	for (int8_t x = 2; x < 10; x++)
		for (int8_t y = 2; y < 10; y++)
			if (get_color(board[x][y]) == color && std::abs(board[x][y]) != Type::White_Pawn)
				generate_piece_moves(x, y, color, ret);
			else if (get_color(board[x][y]) == color && std::abs(board[x][y]) == Type::White_Pawn)
			{
				Color enemy_color = Color(-1 * color);
				Position pos = {x, y};

				auto offsets = get_offsets(at(pos));
				auto offset = ++(++offsets.begin());
				// Offsets de prise en diagonale
				if (Position(x + offset->x, y + offset->y) == en_passant || get_color(at(pos + *offset)) == enemy_color)
					ret.push_back({ {x, y}, Position(x, y) + *offset });
				offset++;
				if (Position(x + offset->x, y + offset->y) == en_passant || get_color(at(pos + *offset)) == enemy_color)
					ret.push_back({ {x, y}, Position(x, y) + *offset });
			}

	return ret;
}
