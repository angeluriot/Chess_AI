#ifndef BOARD_H
#define BOARD_H

#include "utils.h"
#include "Piece.h"

class Board
{
public:

	std::array<std::array<Type, 12>, 12> board;
	std::list<Move> white_moves;
	std::list<Move> black_moves;
	std::map<Position, std::list<Move>> pins;
	Position en_passant;
	Color player_turn;
	std::map<Color, std::array<bool, 2>> allowed_castle;
	Position clicked_cell;

	Board(const std::string& fen);
	Board(const Board& other);

	void operator=(const Board& other);
	Type& operator[](const Position& position);
	inline Type& at(const Position& position);

	inline std::list<Move>& get_player_moves();
	inline std::list<Move>& get_enemy_moves();

	void init_board(const std::string& fen);

	Board get_moved_board(const Move& move);
	int16_t get_score(Color color);
	int16_t move_score(const Move& move, Color color);

	void move_piece(const Move& move);

	void clear_moves();
	void generate_moves(Color color);
	void generate_piece_moves(int8_t x, int8_t y, Color color);
	void generate_pins();
	void remove_illegal_moves(Color color);

	void check_click_on_piece(const sf::RenderWindow& window, float cell_size);
	void draw_moves(sf::RenderWindow& window, std::map<Type, sf::Texture>& textures, float cell_size);
	void draw_pieces(sf::RenderWindow& window, std::map<Type, sf::Texture>& textures, float cell_size);
	void draw_pins(sf::RenderWindow& window, std::map<Type, sf::Texture>& textures, float cell_size);

private:
	void generate_pawn_moves(int8_t x, int8_t y, Color color);
	void handle_castling(const Move& move);
	bool x_ray_attack(Position pos, Position pos2);
	bool is_legal(Move move);
};

#endif
