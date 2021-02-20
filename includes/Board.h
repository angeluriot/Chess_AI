#ifndef BOARD_H
#define BOARD_H

#include "utils.h"
#include "Piece.h"

enum Piece_type : int8_t
{
	no_piece = 0,
	out_of_range = 99,

	White_pawn = 1,
	White_knight = 2,
	White_bishop = 3,
	White_rook = 4,
	White_queen = 5,
	White_king = 6,

	Black_pawn = -1,
	Black_knight = -2,
	Black_bishop = -3,
	Black_rook = -4,
	Black_queen = -5,
	Black_king = -6,
};

class Board
{
public:

	std::array<std::array<Piece_type, 12>, 12> board;
	std::list<Move> white_moves;
	std::list<Move> black_moves;
	Position en_passant;
	PieceColor player_turn;

	Board();
	Board(const Board& other);

	void operator=(const Board& other);
	Piece_type& operator[](Position position);

	uint16_t get_value(Piece_type piece);
	PieceColor get_color(Piece_type piece);
	int16_t get_score(PieceColor color);
	int16_t move_score(const Move& move, PieceColor color);
	void clear_moves();
	void move_piece(Move move);
	void generate_moves(PieceColor color);
	void draw_pieces(sf::RenderWindow& window, float cell_size);
	void draw_moves(sf::RenderWindow& window, float cell_size);
	void check_click_on_piece(const sf::RenderWindow& window, float cell_size);
};

#endif 
