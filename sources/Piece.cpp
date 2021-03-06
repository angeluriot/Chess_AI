#include "utils.h"
#include "Piece.h"
#include "Board.h"

Piece::Piece(PieceType* type, Color color, Position pos, Board* board)
	: type(type), color(color), pos(pos), board(board), enemy_color(color == White ? Black : White)
{
}

Piece::Piece(const Piece& other)
	: type(other.type), color(other.color), pos(other.pos), board(other.board), enemy_color(other.enemy_color)
{
}

Piece::~Piece() {}

Piece& Piece::operator=(const Piece& other)
{
	type = other.type;
	color = other.color;
	pos = other.pos;
	board = other.board;
	enemy_color = other.enemy_color;
	moves.clear();
	return *this;
}

bool Piece::operator==(const Piece& other)
{
	return (board == other.board && pos == other.pos);
}

bool Piece::setPos(const Position& new_pos)
{
	/*if (!new_pos.is_valid())
		return false;
	if ((*board)[new_pos])
		board->pieces.remove(*((*board)[new_pos]));
	(*board)[new_pos] = this;
	(*board)[pos] = NULL;
	pos = new_pos;
	return true;*/
	return true;
}

bool Piece::setPos(int8_t x, int8_t y)
{
	return setPos({x, y});
}

bool Piece::move(const Position& offset)
{
	return setPos(pos + offset);
}

bool Piece::move(int8_t x_offset, int8_t y_offset)
{
	return move({x_offset, y_offset});
}

const Position& Piece::getPos() const
{
	return pos;
}

std::list<Move>& Piece::generatePawnMoves()
{
/*	moves.clear();

	auto offset = type->offsets.begin();
	if ((pos + *offset).is_valid() && !((*board)[pos + *offset]))
	{
		moves.push_back({ pos, pos + *offset });
		offset++;
		if (!((*board)[pos + *offset]) && ((color == White && pos.y == 6) || (color == Black && pos.y == 1)))
			moves.push_back({ pos, pos + *offset });
		offset++;
	}
	else
	{
		offset++;
		offset++;
	}

	if (pos + *offset == board->en_passant || ((pos + *offset).is_valid() && (*board)[pos + *offset] && (*board)[pos + *offset]->color == enemy_color))
		moves.push_back({ pos, pos + *offset });
	offset++;
	if (pos + *offset == board->en_passant || ((pos + *offset).is_valid() && (*board)[pos + *offset] && (*board)[pos + *offset]->color == enemy_color))
		moves.push_back({ pos, pos + *offset });
*/
	return moves;
}

std::list<Move>& Piece::generateMoves()
{
/*	moves.clear();
	if (type->type == PieceType::Type::Pawn)
		return generatePawnMoves();
	for (auto& offset : type->offsets)
	{
		Position actual = pos;
		do
		{
			if (!(actual + offset).is_valid() || ((*board)[actual + offset] && (*board)[actual + offset]->color == color))
				break;
			moves.push_back({pos, actual + offset});
			if ((*board)[actual + offset] && (*board)[actual + offset]->color == enemy_color)
				break;
			actual = actual + offset;
		} while (type->is_linear);
	}
	return moves;*/
	return moves;
}
