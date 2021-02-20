#ifndef BOARD_H
#define BOARD_H

#include "utils.h"
#include "Piece.h"

class Board
{
public:

	std::array<std::array<Piece*, 8>, 8> board;
	std::list<Piece> pieces;
	Position en_passant;
	PieceColor player_turn;
	std::map<PieceColor, std::map<PieceType::Type, sf::Texture>> sprites;
	Position clicked_cell;

	Board();
	Board(const Board& other);

	void operator=(const Board& other);
	Piece*& operator[](Position position);

	uint16_t get_score(PieceColor color);
	int16_t move_score(const Move& move);
	void update_moves(PieceColor color);
	void move_piece(Move move);
	void draw_pieces(sf::RenderWindow& window, float cell_size);
	void draw_moves(sf::RenderWindow& window, float cell_size);
	void check_click_on_piece(const sf::RenderWindow& window, float cell_size);
};

#endif
