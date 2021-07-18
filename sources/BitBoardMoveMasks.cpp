#include "BitBoard.h"

/*
**  ==============================
**
**  Threat Masks
**
**  ==============================
*/

uint64_t BitBoardGlobals::get_knight_threats_mask(uint8_t square) const
{
	uint64_t mask = 0;

	uint64_t square_mask = (1ULL << square);
	if ((square_mask << 17) & not_col[a])
		mask |= (square_mask << 17);
	if ((square_mask << 15) & not_col[h])
		mask |= (square_mask << 15);
	if ((square_mask << 10) & not_col[a] & not_col[b])
		mask |= (square_mask << 10);
	if ((square_mask << 6) & not_col[g] & not_col[h])
		mask |= (square_mask << 6);

	if ((square_mask >> 17) & not_col[h])
		mask |= (square_mask >> 17);
	if ((square_mask >> 15) & not_col[a])
		mask |= (square_mask >> 15);
	if ((square_mask >> 10) & not_col[g] & not_col[h])
		mask |= (square_mask >> 10);
	if ((square_mask >> 6) & not_col[a] & not_col[b])
		mask |= (square_mask >> 6);

	return mask;
}

uint64_t BitBoardGlobals::get_king_threats_mask(uint8_t square) const
{
	uint64_t mask = 0;

	uint64_t square_mask = (1ULL << square);

	if ((square_mask << 1) & not_col[a])
		mask |= (square_mask << 1);
	if ((square_mask << 7) & not_col[h])
		mask |= (square_mask << 7);
	if ((square_mask << 9) & not_col[a])
		mask |= (square_mask << 9);

	if ((square_mask >> 1) & not_col[h])
		mask |= (square_mask >> 1);
	if ((square_mask >> 7) & not_col[a])
		mask |= (square_mask >> 7);
	if ((square_mask >> 9) & not_col[h])
		mask |= (square_mask >> 9);

	return mask | (square_mask << 8) | (square_mask >> 8);
}

uint64_t BitBoardGlobals::get_bishop_threats_mask(uint8_t square) const
{
	uint64_t mask = 0;

	uint8_t start_y = square / 8;
	uint8_t start_x = square % 8;

	for (int8_t x = start_x + 1, y = start_y + 1; x < 7 && y < 7; x++, y++)
		mask |= (1ULL << (y * 8 + x));
	for (int8_t x = start_x - 1, y = start_y - 1; x > 0 && y > 0; x--, y--)
		mask |= (1ULL << (y * 8 + x));

	for (int8_t x = start_x - 1, y = start_y + 1; x > 0 && y < 7; x--, y++)
		mask |= (1ULL << (y * 8 + x));
	for (int8_t x = start_x + 1, y = start_y - 1; x < 7 && y > 0; x++, y--)
		mask |= (1ULL << (y * 8 + x));
	return mask;
}

uint64_t BitBoardGlobals::get_rook_threats_mask(uint8_t square) const
{
	uint64_t mask = 0;

	uint8_t start_y = square / 8;
	uint8_t start_x = square % 8;

	for (int8_t x = start_x + 1; x < 7; x++)
		mask |= (1ULL << (start_y * 8 + x));
	for (int8_t x = start_x - 1; x > 0; x--)
		mask |= (1ULL << (start_y * 8 + x));

	for (int8_t y = start_y + 1; y < 7; y++)
		mask |= (1ULL << (y * 8 + start_x));
	for (int8_t y = start_y - 1; y > 0; y--)
		mask |= (1ULL << (y * 8 + start_x));
	return mask;
}

/*
**  ==============================
**
**  Threat masks with blockers
**
**  ==============================
*/

uint64_t BitBoardGlobals::bishop_threats_with_blockers(uint8_t square, uint64_t blockers) const
{
	uint64_t mask = 0;

	uint8_t start_y = square / 8;
	uint8_t start_x = square % 8;

	for (int8_t x = start_x + 1, y = start_y + 1; x < 8 && y < 8; x++, y++)
	{
		mask |= (1ULL << (y * 8 + x));
		if ((1ULL << (y * 8 + x)) & blockers)
			break;
	}
	for (int8_t x = start_x - 1, y = start_y - 1; x >= 0 && y >= 0; x--, y--)
	{
		mask |= (1ULL << (y * 8 + x));
		if ((1ULL << (y * 8 + x)) & blockers)
			break;
	}

	for (int8_t x = start_x - 1, y = start_y + 1; x >= 0 && y < 8; x--, y++)
	{
		mask |= (1ULL << (y * 8 + x));
		if ((1ULL << (y * 8 + x)) & blockers)
			break;
	}

	for (int8_t x = start_x + 1, y = start_y - 1; x < 8 && y >= 0; x++, y--)
	{
		mask |= (1ULL << (y * 8 + x));
		if ((1ULL << (y * 8 + x)) & blockers)
			break;
	}
	return mask;
}

uint64_t BitBoardGlobals::rook_threats_with_blockers(uint8_t square, uint64_t blockers) const
{
	uint64_t mask = 0;

	uint8_t start_y = square / 8;
	uint8_t start_x = square % 8;

	for (int8_t x = start_x + 1; x < 8; x++)
	{
		mask |= (1ULL << (start_y * 8 + x));
		if ((1ULL << (start_y * 8 + x)) & blockers)
			break;
	}

	for (int8_t x = start_x - 1; x >= 0; x--)
	{
		mask |= (1ULL << (start_y * 8 + x));
		if ((1ULL << (start_y * 8 + x)) & blockers)
			break;
	}

	for (int8_t y = start_y + 1; y < 8; y++)
	{
		mask |= (1ULL << (y * 8 + start_x));
		if ((1ULL << (y * 8 + start_x)) & blockers)
			break;
	}

	for (int8_t y = start_y - 1; y >= 0; y--)
	{
		mask |= (1ULL << (y * 8 + start_x));
		if ((1ULL << (y * 8 + start_x)) & blockers)
			break;
	}

	return mask;
}

uint64_t BitBoardGlobals::set_occupancy(uint64_t index, uint8_t bits_in_mask, uint64_t attack_mask) const
{
	uint64_t occupancy = 0ULL;

	for (uint8_t count = 0; count < bits_in_mask; count++)
	{
		uint8_t square = get_least_significant_bit(attack_mask);
		pop_bit(attack_mask, square);

		if (index & (1ULL << count))
			occupancy |= (1ULL << square);
	}
	return occupancy;
}
