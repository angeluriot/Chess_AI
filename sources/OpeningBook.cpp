#include "OpeningBook.h"
#include <stdexcept>

OpeningBook::OpeningBook(const std::string& name) : book_size(0)
{
	file.open(name, std::ifstream::binary);

	if (!file.good() || !file.is_open())
		throw std::runtime_error("A");

	file.seekg(0, std::ios::end);
	if (!file.good())
		throw std::runtime_error("B");

	book_size = file.tellg();
	if (book_size == -1)
		throw std::runtime_error("C");
	book_size /= 16;
}

OpeningBook::~OpeningBook()
{
	if (file.is_open())
		file.close();
}

std::pair<uint16_t, int> OpeningBook::book_move(BitBoard& board)
{
	OpeningBookEntry entry;
	std::list<uint16_t> list;
	uint64_t hash = board.signature_hash();

	if (!file.good() || !file.is_open() || book_size == 0)
		throw std::runtime_error("D");

	int best_move = -1;
	int best_score = 0;
	for (int pos = find_pos(hash); pos < book_size; pos++)
	{
		read_entry(entry,pos);
		if (entry.key != hash) break;

		if (entry.count <= 0)
			throw std::runtime_error("E");

		best_score += entry.count;
		if (rand() % best_score < entry.count)
			best_move = entry.move;
	}

	if (best_move != -1)
	{
		uint8_t target_x = best_move & 0b111;
		uint8_t target_y = 7 - ((best_move & 0b111000) >> 3);
		uint8_t start_x =  (best_move & 0b111000000) >> 6;
		uint8_t start_y =  7 - ((best_move & 0b111000000000) >> 9);
		if (get_bit(board.piece_boards[Piece::White_King] | board.piece_boards[Piece::Black_King], start_y * 8 + start_x))
		{
			if (start_x == e && target_x == a)
				target_x = c;
			if (start_x == e && target_x == h)
				target_x = g;
		}
		return std::make_pair(((start_y * 8 + start_x) << 8) | (target_y * 8 + target_x), best_score);
	}
	return std::make_pair(No_Square, 0);
}

int OpeningBook::find_pos(uint64_t key)
{
	int left = 0, right = book_size - 1, mid;
	OpeningBookEntry entry;

	if (left > right)
		throw std::runtime_error("F");

	while (left < right)
	{
		mid = (left + right) / 2;
		if (mid < left || mid >= right)
			throw std::exception();

		read_entry(entry, mid);

		if (key <= entry.key)
			right = mid;
		else
			left = mid + 1;
	}

	if (left != right)
		throw std::exception();

	read_entry(entry, left);

	return entry.key == key ? left : book_size;
}


void OpeningBook::read_entry(OpeningBookEntry& entry, int n)
{
	if (n < 0 || n >= book_size)
		throw std::exception();

	if (!file.good() || !file.is_open())
		throw std::exception();

	file.seekg(n * 16);
	if (file.fail())
		throw std::exception();

	entry.key   = read_integer(8);
	entry.move  = read_integer(2);
	entry.count = read_integer(2);
	entry.n     = read_integer(2);
	entry.sum   = read_integer(2);
}


uint64_t OpeningBook::read_integer(int size)
{
	uint64_t n = 0;

	if (!file.good() || !file.is_open())
		throw std::exception();

	for (int i = 0; i < size; i++)
	{
		int byte = file.get();
		if (byte < 0 || byte >= 256)
			throw std::exception();
		n = (n << 8) | byte;
	}
	return n;
}
