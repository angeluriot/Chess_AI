#include "utils.h"
#include "Piece.h"
#include "Board.h"

Piece::Piece(PieceType& type, PieceColor color, Position pos, Board* board)
	: type(type), color(color), pos(pos), board(board), enemy_color(color == White ? Black : White)
{
	(*board)[pos] = this;
}
Piece::Piece(const Piece& other)
	: type(other.type), color(other.color), pos(other.pos), board(other.board), enemy_color(other.enemy_color)
{
	(*board)[pos] = this;
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

bool Piece::setPos(const Position& position)
{
	if (position.is_valid())
		return false;
	(*board)[pos] = NULL;
	pos = position;
	if ((*board)[pos])
		board->pieces.remove(*((*board)[pos]));
	(*board)[pos] = this;
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
	moves.clear();

	auto offset = type.offsets.begin();
	if ((pos + *offset).is_valid() && !(*board)[pos + *offset])
	{
		moves.push_back({ pos, pos + *offset, board });
		offset++;
		if ((pos + *offset).is_valid() && !(*board)[pos + *offset] && ((color == White && pos.y == 6) || (color == Black && pos.y == 1)))
			moves.push_back({ pos, pos + *offset, board });
		offset++;
	}

	if (pos + *offset == board->en_passant || ((pos + *offset).is_valid() && (*board)[pos + *offset] && (*board)[pos + *offset]->color == enemy_color))
		moves.push_back({ pos, pos + *offset, board });
	offset++;
	if (pos + *offset == board->en_passant || ((pos + *offset).is_valid() && (*board)[pos + *offset] && (*board)[pos + *offset]->color == enemy_color))
		moves.push_back({ pos, pos + *offset, board });

	return moves;
}

std::list<Move>& Piece::generateMoves()
{
	moves.clear();
	if (type.type == PieceType::Type::Pawn)
		return generatePawnMoves();
	for (auto& offset : type.offsets)
	{
		Position actual = pos;
		do
		{
			if (!(actual + offset).is_valid() || ((*board)[actual + offset] && (*board)[actual + offset]->color == color))
				break;
			moves.push_back({actual, actual + offset, board});
			if ((*board)[actual + offset] && (*board)[actual + offset]->color == enemy_color)
				break;
			actual = actual + offset;
		} while (type.is_linear);
	}
	return moves;
}
