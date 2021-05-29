#include "BitBoard.h"

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

Color get_color(Piece piece)
{
	if (piece == Piece::No_Piece)
		return Color::No_Color;
	return (piece > 0 ? Color::White : Color::Black);
}
