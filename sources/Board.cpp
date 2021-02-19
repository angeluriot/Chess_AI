#include "Board.h"

Board::Board()
{
	pieces.clear();

	for (auto& square : board)
		std::fill(square.begin(), square.end(), PIECE_NULL);

	pieces.assign({
		Piece(PieceType::rook, Black, { 0, 0 }, this),
		Piece(PieceType::knight, Black, { 1, 0 }, this),
		Piece(PieceType::bishop, Black, { 2, 0 }, this),
		Piece(PieceType::queen, Black, { 3, 0 }, this),
		Piece(PieceType::king, Black, { 4, 0 }, this),
		Piece(PieceType::bishop, Black, { 5, 0 }, this),
		Piece(PieceType::knight, Black, { 6, 0 }, this),
		Piece(PieceType::rook, Black, { 7, 0 }, this),

		Piece(PieceType::black_pawn, Black, { 0, 1 }, this),
		Piece(PieceType::black_pawn, Black, { 1, 1 }, this),
		Piece(PieceType::black_pawn, Black, { 2, 1 }, this),
		Piece(PieceType::black_pawn, Black, { 3, 1 }, this),
		Piece(PieceType::black_pawn, Black, { 4, 1 }, this),
		Piece(PieceType::black_pawn, Black, { 5, 1 }, this),
		Piece(PieceType::black_pawn, Black, { 6, 1 }, this),
		Piece(PieceType::black_pawn, Black, { 7, 1 }, this),

		Piece(PieceType::white_pawn, White, { 0, 6 }, this),
		Piece(PieceType::white_pawn, White, { 1, 6 }, this),
		Piece(PieceType::white_pawn, White, { 2, 6 }, this),
		Piece(PieceType::white_pawn, White, { 3, 6 }, this),
		Piece(PieceType::white_pawn, White, { 4, 6 }, this),
		Piece(PieceType::white_pawn, White, { 5, 6 }, this),
		Piece(PieceType::white_pawn, White, { 6, 6 }, this),
		Piece(PieceType::white_pawn, White, { 7, 6 }, this),

		Piece(PieceType::rook, White, { 0, 7 }, this),
		Piece(PieceType::knight, White, { 1, 7 }, this),
		Piece(PieceType::bishop, White, { 2, 7 }, this),
		Piece(PieceType::queen, White, { 3, 7 }, this),
		Piece(PieceType::king, White, { 4, 7 }, this),
		Piece(PieceType::bishop, White, { 5, 7 }, this),
		Piece(PieceType::knight, White, { 6, 7 }, this),
		Piece(PieceType::rook, White, { 7, 7 }, this)
		});

	en_passant = Position::invalid;
}

Board::Board(const Board& other)
{
	*this = other;
}

void Board::operator=(const Board& other)
{
	pieces = other.pieces;

	for (auto& square : board)
		std::fill(square.begin(), square.end(), PIECE_NULL);

	for (auto& piece : pieces)
		(*this)[piece.pos] = &piece;

	en_passant = other.en_passant;
}

Piece*& Board::operator[](Position position)
{
	return board[position.x][position.y];
}

uint16_t Board::get_score(PieceColor color)
{
	return std::accumulate(pieces.begin(), pieces.end(), uint16_t(), [color](uint16_t sum, Piece& piece) -> uint16_t { return sum + (piece.color == color ? piece.type.value : 0); });
}

void Board::update_moves()
{
	for (auto& piece : pieces)
		piece.generateMoves();
}

void Board::move_piece(Move move)
{
	(*this)[move.start]->setPos(move.target);
}
