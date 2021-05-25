#ifndef BOARD_H
#define BOARD_H

#include "utils.h"
#include "Piece.h"

static const std::map<Type, std::array<int, 64>> tables
{
	{ Type::White_Pawn, {	0,  0,  0,  0,  0,  0,  0,  0,
							50, 50, 50, 50, 50, 50, 50, 50,
							10, 10, 20, 30, 30, 20, 10, 10,
							5,  5, 10, 25, 25, 10,  5,  5,
							0,  0,  0, 20, 20,  0,  0,  0,
							5, -5,-10,  0,  0,-10, -5,  5,
							5, 10, 10,-20,-20, 10, 10,  5,
							0,  0,  0,  0,  0,  0,  0,  0 } },

	{ Type::Black_Pawn, {	0,  0,  0,  0,  0,  0,  0,  0,
							5, 10, 10,-20,-20, 10, 10,  5,
							5, -5,-10,  0,  0,-10, -5,  5,
							0,  0,  0, 20, 20,  0,  0,  0,
							5,  5, 10, 25, 25, 10,  5,  5,
							10, 10, 20, 30, 30, 20, 10, 10,
							50, 50, 50, 50, 50, 50, 50, 50,
							0,  0,  0,  0,  0,  0,  0,  0 } },

	{ Type::White_Knight, {	-50,-40,-30,-30,-30,-30,-40,-50,
							-40,-20,  0,  0,  0,  0,-20,-40,
							-30,  0, 10, 15, 15, 10,  0,-30,
							-30,  5, 15, 20, 20, 15,  5,-30,
							-30,  0, 15, 20, 20, 15,  0,-30,
							-30,  5, 10, 15, 15, 10,  5,-30,
							-40,-20,  0,  5,  5,  0,-20,-40,
							-50,-40,-30,-30,-30,-30,-40,-50 } },

	{ Type::Black_Knight, {	-50,-40,-30,-30,-30,-30,-40,-50,
							-40,-20,  0,  5,  5,  0,-20,-40,
							-30,  5, 10, 15, 15, 10,  5,-30,
							-30,  0, 15, 20, 20, 15,  0,-30,
							-30,  5, 15, 20, 20, 15,  5,-30,
							-30,  0, 10, 15, 15, 10,  0,-30,
							-40,-20,  0,  0,  0,  0,-20,-40,
							-50,-40,-30,-30,-30,-30,-40,-50 } },


	{ Type::White_Bishop, {	-20,-10,-10,-10,-10,-10,-10,-20,
							-10,  0,  0,  0,  0,  0,  0,-10,
							-10,  0,  5, 10, 10,  5,  0,-10,
							-10,  5,  5, 10, 10,  5,  5,-10,
							-10,  0, 10, 10, 10, 10,  0,-10,
							-10, 10, 10, 10, 10, 10, 10,-10,
							-10,  5,  0,  0,  0,  0,  5,-10,
							-20,-10,-10,-10,-10,-10,-10,-20 } },

	{ Type::Black_Bishop, {	-20,-10,-10,-10,-10,-10,-10,-20,
							-10,  5,  0,  0,  0,  0,  5,-10,
							-10, 10, 10, 10, 10, 10, 10,-10,
							-10,  0, 10, 10, 10, 10,  0,-10,
							-10,  5,  5, 10, 10,  5,  5,-10,
							-10,  0,  5, 10, 10,  5,  0,-10,
							-10,  0,  0,  0,  0,  0,  0,-10,
							-20,-10,-10,-10,-10,-10,-10,-20 } },

	{ Type::White_Rook, {	0,  0,  0,  0,  0,  0,  0,  0,
							5, 10, 10, 10, 10, 10, 10,  5,
							-5,  0,  0,  0,  0,  0,  0, -5,
							-5,  0,  0,  0,  0,  0,  0, -5,
							-5,  0,  0,  0,  0,  0,  0, -5,
							-5,  0,  0,  0,  0,  0,  0, -5,
							-5,  0,  0,  0,  0,  0,  0, -5,
							0,  0,  0,  5,  5,  0,  0,  0 } },

	{ Type::Black_Rook, {	0,  0,  0,  5,  5,  0,  0,  0,
							-5,  0,  0,  0,  0,  0,  0, -5,
							-5,  0,  0,  0,  0,  0,  0, -5,
							-5,  0,  0,  0,  0,  0,  0, -5,
							-5,  0,  0,  0,  0,  0,  0, -5,
							-5,  0,  0,  0,  0,  0,  0, -5,
							5, 10, 10, 10, 10, 10, 10,  5,
							0,  0,  0,  0,  0,  0,  0,  0 } },

	{ Type::White_Queen, {	-20,-10,-10, -5, -5,-10,-10,-20,
							-10,  0,  0,  0,  0,  0,  0,-10,
							-10,  0,  5,  5,  5,  5,  0,-10,
							 -5,  0,  5,  5,  5,  5,  0, -5,
							  0,  0,  5,  5,  5,  5,  0, -5,
							-10,  5,  5,  5,  5,  5,  0,-10,
							-10,  0,  5,  0,  0,  0,  0,-10,
							-20,-10,-10, -5, -5,-10,-10,-20 } },

	{ Type::Black_Queen, {	-20,-10,-10, -5, -5,-10,-10,-20,
							-10,  0,  5,  0,  0,  0,  0,-10,
							-10,  5,  5,  5,  5,  5,  0,-10,
							  0,  0,  5,  5,  5,  5,  0, -5,
							 -5,  0,  5,  5,  5,  5,  0, -5,
							-10,  0,  5,  5,  5,  5,  0,-10,
							-10,  0,  0,  0,  0,  0,  0,-10,
							-20,-10,-10, -5, -5,-10,-10,-20 } },

	{ Type::White_King, {	-30,-40,-40,-50,-50,-40,-40,-30,
							-30,-40,-40,-50,-50,-40,-40,-30,
							-30,-40,-40,-50,-50,-40,-40,-30,
							-30,-40,-40,-50,-50,-40,-40,-30,
							-20,-30,-30,-40,-40,-30,-30,-20,
							-10,-20,-20,-20,-20,-20,-20,-10,
							20, 20,  0,  0,  0,  0, 20, 20,
							20, 30, 10,  0,  0, 10, 30, 20 } },

	{ Type::Black_King, {	20, 30, 10,  0,  0, 10, 30, 20,
							20, 20,  0,  0,  0,  0, 20, 20,
							-10,-20,-20,-20,-20,-20,-20,-10,
							-20,-30,-30,-40,-40,-30,-30,-20,
							-30,-40,-40,-50,-50,-40,-40,-30,
							-30,-40,-40,-50,-50,-40,-40,-30,
							-30,-40,-40,-50,-50,-40,-40,-30,
							-30,-40,-40,-50,-50,-40,-40,-30 } },
};

class Board
{
public:

	std::array<std::array<Type, 12>, 12> board;
	Position en_passant;
	Color player_turn;
	std::map<Color, std::array<bool, 2>> allowed_castle;
	Position clicked_cell;
	Move last_move;

	Board(const std::string& fen);
	Board(const Board& other);

	void operator=(const Board& other);
	Type& operator[](const Position& position);
	inline Type& at(const Position& position) { return board[position.x][position.y]; }
	uint8_t count_piece(const Type& type);
	bool is_endgame();

	void init_board(const std::string& fen);

	Board get_moved_board(const Move& move) const;
	int get_score();

	void move_piece(const Move& move);

	bool is_finished();

	std::list<Move> generate_moves(Color color);
	std::list<Move> generate_threats(Color color);
	void generate_piece_moves(int8_t x, int8_t y, Color color, std::list<Move>& ret, const std::list<Move>& threats = std::list<Move>());

	void check_click_on_piece(const sf::RenderWindow& window, float cell_size, Board* last_board);
	void draw_moves(sf::RenderWindow& window, std::map<Type, sf::Texture>& textures, float cell_size);
	void draw_pieces(sf::RenderWindow& window, std::map<Type, sf::Texture>& textures, float cell_size);
	void draw_pins(sf::RenderWindow& window, std::map<Type, sf::Texture>& textures, float cell_size);
	void draw_last_move(sf::RenderWindow& window, std::map<Type, sf::Texture>& textures, float cell_size);

private:
	void generate_pawn_moves(int8_t x, int8_t y, Color color, std::list<Move>& ret);
	void handle_castling(const Move& move);

};

#endif
