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

	uint8_t white_bishop = 0, black_bishop = 0;

	// Valeur des pièces et activité
	for (uint8_t i = 2; i < 10; i++)
	{
		for (uint8_t j = 2; j < 10; j++)
		{
			Type& type = board[i][j];
			if (type == Type::No_Piece)
				continue;

			// Bishop pair
			if (type == Type::White_Bishop)
				white_bishop++;
			else if (type == Type::Black_Bishop)
				black_bishop++;

			// Knight supported by a pawn
			if (type == Type::White_Knight && (board[i + 1][j + 1] == Type::White_Pawn || board[i - 1][j + 1] == Type::White_Pawn))
				score += 20;
			else if (type == Type::Black_Knight && (board[i + 1][j - 1] == Type::Black_Pawn || board[i - 1][j - 1] == Type::Black_Pawn))
				score += 20;

			// Pions passés
			if (board[i][j] == Type::White_Pawn &&
					std::count(board[i - 1].rbegin() + (12 - j), board[i - 1].rend(), Type::Black_Pawn) == 0 &&
					std::count(board[i + 1].rbegin() + (12 - j), board[i + 1].rend(), Type::Black_Pawn) == 0 &&
					std::count_if(board[i].rbegin() + (12 - j), board[i].rend(), [](const Type& val) { return std::abs(val) == Type::White_Pawn; }) == 0)
				score += 50 + (10 - j) * 30;
			else if (board[i][j] == Type::Black_Pawn &&
					std::count(board[i - 1].begin() + j, board[i - 1].end(), Type::White_Pawn) == 0 &&
					std::count(board[i + 1].begin() + j, board[i + 1].end(), Type::White_Pawn) == 0 &&
					std::count_if(board[i].begin() + j, board[i].end(), [](const Type& val) { return std::abs(val) == Type::Black_Pawn; }) == 0)
				score -= 50 + (j - 2) * 30;


			// Matériel
			score += get_value(type);

			int abs = std::abs(type);
			if (!(half_turn > 40 && abs == Type::White_King))
				score += tables.find(type)->second[(j - 2) * 8 + (i - 2)] * get_color(type);
		}

		// Pions doublés
		score -= 30 * (static_cast<int>(std::count(board[i].begin(), board[i].end(), Type::White_Pawn)) >= 2 ? 0 : 1);
		score += 30 * (static_cast<int>(std::count(board[i].begin(), board[i].end(), Type::Black_Pawn)) >= 2 ? 0 : 1);
	}

	if (white_bishop >= 2)
		score += 25;
	if (black_bishop >= 2)
		score -= 25;

/*	for (uint8_t i = 2; i < 10; i++)
	{
		bool white_isolated = false, black_isolated = false;

		for (uint8_t j = 2; j < 10; j++)
		{
			// Pions isolés
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
			}

			// Pions bloqués
			if (board[i][j] == Type::White_Pawn && board[i][j - 1] != Type::No_Piece)
				score -= 30;
			else if (board[i][j] == Type::Black_Pawn && board[i][j + 1] != Type::No_Piece)
				score += 30;

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
	}*/
	return score;
}

uint64_t Board::get_key(uint8_t x, uint8_t y, Type type)
{
	int cell_index = (7 - (y - 2)) * 8 + (x - 2);
	switch (type)
	{
		case Type::Black_Pawn:
			return zobrist_keys[64 * 0 + cell_index];
		case Type::White_Pawn:
			return zobrist_keys[64 * 1 + cell_index];
		case Type::Black_Knight:
			return zobrist_keys[64 * 2 + cell_index];
		case Type::White_Knight:
			return zobrist_keys[64 * 3 + cell_index];
		case Type::Black_Bishop:
			return zobrist_keys[64 * 4 + cell_index];
		case Type::White_Bishop:
			return zobrist_keys[64 * 5 + cell_index];
		case Type::Black_Rook:
			return zobrist_keys[64 * 6 + cell_index];
		case Type::White_Rook:
			return zobrist_keys[64 * 7 + cell_index];
		case Type::Black_Queen:
			return zobrist_keys[64 * 8 + cell_index];
		case Type::White_Queen:
			return zobrist_keys[64 * 9 + cell_index];
		case Type::Black_King:
			return zobrist_keys[64 * 10 + cell_index];
		case Type::White_King:
			return zobrist_keys[64 * 11 + cell_index];
		default:
			return 0;
	}
	return 0;
}

uint64_t Board::signature_hash()
{
	uint64_t ret = 0;
	for (int x = 2; x < 10; x++)
	{
		for (int y = 2; y < 10; y++)
		{
			if (board[x][y] == Type::No_Piece)
				continue;
			ret ^= get_key(x, y, board[x][y]);
		}
	}

	if (allowed_castle[Color::White][1])
		ret ^= zobrist_keys[64 * 12 + 0];
	if (allowed_castle[Color::White][0])
		ret ^= zobrist_keys[64 * 12 + 1];
	if (allowed_castle[Color::Black][1])
		ret ^= zobrist_keys[64 * 12 + 2];
	if (allowed_castle[Color::Black][0])
		ret ^= zobrist_keys[64 * 12 + 3];

	uint64_t key_en_passant = 0;
	if (en_passant != Position::invalid &&
			((std::abs(board[en_passant.x - 1][en_passant.y]) == Type::White_Pawn && get_color(board[en_passant.x - 1][en_passant.y]) != get_color(board[en_passant.x][en_passant.y])) ||
			(std::abs(board[en_passant.x + 1][en_passant.y]) == Type::White_Pawn && get_color(board[en_passant.x + 1][en_passant.y]) != get_color(board[en_passant.x][en_passant.y]))))
		key_en_passant ^= zobrist_keys[64 * 12 + 4 + (en_passant.x - 2)];

	if (player_turn == Color::White)
		ret ^= zobrist_keys[64 * 12 + 4 + 8];
	return ret;
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
