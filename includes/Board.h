#ifndef BOARD_H
#define BOARD_H

#include "utils.h"
#include "Piece.h"

enum Piece_type : int8_t
{
	No_piece = 0,
	Out_of_range = 99,

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
	Color player_turn;
	std::map<Piece_type, sf::Texture> textures;
	std::map<Color, std::array<bool, 2>> allowed_castle;
	Position clicked_cell;

	Board();
	Board(const Board& other);

	void operator=(const Board& other);
	Piece_type& operator[](Position position);

	uint16_t get_value(Piece_type piece);
	Color get_color(Piece_type piece);
	int16_t get_score(Color color);
	int16_t move_score(const Move& move, Color color);
	void clear_moves();
	void move_piece(Move move);
	void generate_moves(Color color);
	void draw_pieces(sf::RenderWindow& window, float cell_size);
	void draw_moves(sf::RenderWindow& window, float cell_size);
	void check_click_on_piece(const sf::RenderWindow& window, float cell_size);
	void generate_piece_moves(int8_t x, int8_t y, Color color);
	void generate_pawn_moves(int8_t x, int8_t y, Color color);
	const std::list<Position>& get_offsets(Piece_type type);
	void handle_castling(Move move);
};

#endif
