#include "Board.h"

Type& Board::operator[](const Position& position)
{
	return board[position.x][position.y];
}

int16_t Board::get_score(Color color)
{
	uint16_t score = 0;

	for (uint8_t i = 2; i < 10; i++)
		for (uint8_t j = 2; j < 10; j++)
		{
			std::list<Move> possible_moves;
			generate_piece_moves(i, j, get_color(board[i][j]), possible_moves);
			score += (get_value(board[i][j], color) * 10 + (possible_moves.size() * get_color(board[i][j])));
		}
	return score;
}

Board Board::get_moved_board(const Move& move)
{
	Board result(*this);
	Color c = get_color(at(move.start));
	result.move_piece(move);
	result.player_turn = Color(-c);
	return result;
}


bool Board::is_finished() const
{
	uint8_t kings = 0;

	for (uint8_t i = 2; i < 10; i++)
		for (uint8_t j = 2; j < 10; j++)
			if (std::abs(board[i][j]) == Type::White_King)
				kings++;
	if (kings != 2)
		return true;
	return false;
}
