#include "utils.h"
#include "Piece.h"

Piece::Piece(PieceType& type, PieceColor color, Position pos, Board* board)
	: type(type), color(color), pos(pos), board(board), enemy_color(color == White ? Black : White) {}
Piece::Piece(const Piece& other)
	: type(other.type), color(other.color), pos(other.pos), board(other.board), enemy_color(other.enemy_color) {}

Piece& Piece::operator=(const Piece& other)
{
	type = other.type;
	color = other.color;
	pos = other.pos;
	board = other.board;
	enemy_color = other.enemy_color;
	return *this;
}

Piece::~Piece() {}

std::list<Move> generateMoves();

bool Piece::setPos(const Position& position)
{
	if (Position::is_valid(position))
		return false;
	(*board)[pos] = NULL;
	this->pos = position;
	(*board)[pos] = this;
	return true;
}

bool Piece::setPos(int8_t x, int8_t y)
{
	return setPos({x, y});
}

bool Piece::move(const Position& offset)
{
	return setPos(this->pos + offset);
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
	if (Position::is_valid(pos + *offset) && !(*board)[pos + *offset])
		moves.push_back({ pos, pos + *offset });
	offset++;

	if (pos + *offset == board->en_passant || (Position::is_valid(pos + *offset) && (*board)[pos + *offset] && (*board)[pos + *offset]->color == enemy_color))
		moves.push_back({ pos, pos + *offset });
	offset++;
	if (pos + *offset == board->en_passant || (Position::is_valid(pos + *offset) && (*board)[pos + *offset] && (*board)[pos + *offset]->color == enemy_color))
		moves.push_back({ pos, pos + *offset });
	offset++;

	if ((color == White && pos.y == 7) || (color == Black && pos.y == 2))
		moves.push_back({ pos, pos + *offset });
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
			if (!Position::is_valid(actual + pos) || ((*board)[pos] && (*board)[pos]->color == color))
				break;
			moves.push_back({actual, actual + offset});
			if ((*board)[pos] && (*board)[pos]->color == color)
				break;
		} while (type.is_linear);
	}
	return moves;
}
