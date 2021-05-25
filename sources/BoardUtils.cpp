#include "Board.h"
#include <algorithm>
#include <chrono>

Type& Board::operator[](const Position& position)
{
	return board[position.x][position.y];
}

uint8_t Board::count_piece(const Type& type)
{
	uint8_t cnt = 0;
	for (uint8_t i = 2; i < 10; i++)
	{
		for (uint8_t j = 2; j < 10; j++)
			if (board[i][j] == type)
				cnt++;
	}
	return cnt;
}

int Board::get_score()
{
	int score = 0;

	// Valeur des pièces et activité
	for (uint8_t i = 2; i < 10; i++)
	{
		for (uint8_t j = 2; j < 10; j++)
		{
			if (board[i][j] == Type::No_Piece)
				continue;
			if (std::abs(board[i][j]) == Type::White_Pawn)
				score += 20 * get_color(board[i][j]);
			else
			{
				std::list<Move> possible_moves;
				generate_piece_moves(i, j, get_color(board[i][j]), possible_moves);
				score += (possible_moves.size() * get_color(board[i][j]) * 10);
			}
			score += get_value(board[i][j]);
			score += tables.find(board[i][j])->second[(j - 2) * 8 + (i - 2)] * get_color(board[i][j]);
		}
	}

	// Pions doublés
	for (uint8_t i = 2; i < 10; i++)
	{
		score -= 20 * std::min(static_cast<int>(std::count(board[i].begin(), board[i].end(), Type::White_Pawn)) - 1, 1);
		score += 20 * std::min(static_cast<int>(std::count(board[i].begin(), board[i].end(), Type::Black_Pawn)) - 1, 1);
	}

	for (uint8_t i = 2; i < 10; i++)
	{
		bool white_isolated = false, black_isolated = false;

		for (uint8_t j = 2; j < 10; j++)
		{
/*			// Pions isolés
			if (!white_isolated && board[i][j] == Type::White_Pawn &&
				std::count(board[i - 1].begin(), board[i - 1].end(), Type::White_Pawn) == 0 &&
				std::count(board[i + 1].begin(), board[i + 1].end(), Type::White_Pawn) == 0)
			{
				score -= 30;
				white_isolated = true;
			}
			else if (!black_isolated && board[i][j] == Type::Black_Pawn &&
				std::count(board[i - 1].begin(), board[i - 1].end(), Type::Black_Pawn) == 0 &&
				std::count(board[i + 1].begin(), board[i + 1].end(), Type::Black_Pawn) == 0)
			{
				black_isolated = true;
				score += 30;
			}*/

/*			// Pions bloqués
			if (board[i][j] == Type::White_Pawn && board[i][j - 1] != Type::No_Piece)
				score -= 30;
			else if (board[i][j] == Type::Black_Pawn && board[i][j + 1] != Type::No_Piece)
				score += 30;*/

			if (board[i][j] == Type::White_Pawn &&
					std::count(board[i - 1].rbegin() + (12 - j), board[i - 1].rend(), Type::Black_Pawn) == 0 &&
					std::count(board[i + 1].rbegin() + (12 - j), board[i + 1].rend(), Type::Black_Pawn) == 0 &&
					std::count_if(board[i].rbegin() + (12 - j), board[i].rend(), [](const Type& val) { return std::abs(val) == Type::White_Pawn; }) == 0)
				score += 50;
			else if (board[i][j] == Type::Black_Pawn &&
				std::count(board[i - 1].begin() + j, board[i - 1].end(), Type::White_Pawn) == 0 &&
				std::count(board[i + 1].begin() + j, board[i + 1].end(), Type::White_Pawn) == 0 &&
				std::count_if(board[i].begin() + j, board[i].end(), [](const Type& val) { return std::abs(val) == Type::Black_Pawn; }) == 0)
				score -= 50;
		}
	}
	return score;
}

Board Board::get_moved_board(const Move& move) const
{
	Board result(*this);
	Color c = get_color(board[move.start.x][move.start.y]);
	result.move_piece(move);
	result.player_turn = Color(c * -1);
	return result;
}


bool Board::is_finished()
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
