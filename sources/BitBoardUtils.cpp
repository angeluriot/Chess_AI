#include "BitBoard.h"
#include "Piece.h"

/*
**  ==============================
**
**  Board utils
**
**  ==============================
*/

uint8_t count_bits(uint64_t board)
{
	uint8_t count = 0;
	while (board != 0)
	{
		board &= board - 1;
		count++;
	}
	return count;
}

uint8_t get_least_significant_bit(uint64_t board)
{
	if (board == 0)
		return 255;
	return count_bits((board & -board) - 1);
}

void print_bitboard(uint64_t board)
{
	for (int y = 0; y < 8; y++)
	{
		std::cout << 8 - y << "  ";
		for (int x = 0; x < 8; x++)
			std::cout << get_bit(board, y * 8 + x) << ' ';
		std::cout << std::endl;
	}
	std::cout << "\n   a b c d e f g h\n\nBitboard: " << board << std::endl;
}

int BitBoard::get_score(const BitBoardGlobals& globals)
{
	int score = 0;

	for (uint8_t i = 0; i < 12; i++)
		score += get_value(i) * count_bits(piece_boards[i]);

	Color side_save = side_to_move;

	side_to_move = Color::White;
	score += generate_moves(globals).size();
	side_to_move = Color::Black;
	score -= generate_moves(globals).size();
	side_to_move = side_save;

/*	uint8_t white_bishop = 0, black_bishop = 0;

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
		score -= 25;*/

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
