#ifndef OPENING_BOOK_H
#define OPENING_BOOK_H

#include <cstdint>
#include <fstream>
#include "BitBoard.h"
#include <map>

struct OpeningBookEntry {
	uint64_t key;
	uint16_t move;
	uint16_t count;
	uint16_t n;
	uint16_t sum;
};

class OpeningBook
{
	private:
		std::ifstream file;
		int book_size;

	public:
		OpeningBook(const std::string& name);
		virtual ~OpeningBook();

		std::pair<uint16_t, int> book_move(BitBoard& board);

	private:
		int find_pos(uint64_t key);
		void read_entry(OpeningBookEntry& entry, int n);
		uint64_t read_integer(int size);
};

#endif
