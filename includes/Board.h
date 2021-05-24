#ifndef BOARD_H
#define BOARD_H

#include "utils.h"
#include "Piece.h"

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

	void init_board(const std::string& fen);

	Board get_moved_board(const Move& move);
	int16_t get_score(Color color);

	void move_piece(const Move& move);

	bool is_finished() const;

	std::list<Move> generate_moves(Color color);
	void generate_piece_moves(int8_t x, int8_t y, Color color, std::list<Move>& ret);

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
