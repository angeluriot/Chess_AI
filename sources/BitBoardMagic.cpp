#include "BitBoard.h"

uint64_t BitBoardGlobals::find_magic_number(uint8_t square, uint8_t relevant, bool is_rook) const
{
	std::array<uint64_t, 4096> occupancies, attacks, used_attacks;

	uint64_t attack_mask = is_rook ? get_rook_threats_mask(square) : get_bishop_threats_mask(square);

	uint64_t occupancy_indices = 1 << relevant;

	for (uint64_t index = 0; index < occupancy_indices; index++)
	{
		occupancies[index] = set_occupancy(index, relevant, attack_mask);
		attacks[index] = is_rook ? rook_threats_with_blockers(square, occupancies[index]) : bishop_threats_with_blockers(square, occupancies[index]);
	}

	for (int count = 0; count < 1000000000; count++)
	{
		uint64_t magic_number = magic_number_candidate();

		if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6)
			continue;

		std::fill(used_attacks.begin(), used_attacks.end(), 0);

		uint64_t index = 0;
		bool fail = false;

		for (; !fail && index < occupancy_indices; index++)
		{
			uint64_t magic_index = (occupancies[index] * magic_number) >> (64 - relevant);

			if (used_attacks[magic_index] == 0)
				used_attacks[magic_index] = attacks[index];
			else if (used_attacks[magic_index] != attacks[index])
				fail = true;
		}

		if (!fail)
			return magic_number;
	}

	std::cerr << "Magic number failed" << std::endl;
	return 0ULL;
}
