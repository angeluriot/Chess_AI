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

int BitBoard::get_score()
{
	int score = 0;

	for (uint8_t i = 0; i < 12; i++)
	{
		uint64_t current_piece_board = piece_boards[i];
		while (current_piece_board)
		{
			uint8_t square = get_least_significant_bit(current_piece_board);
			uint8_t table_square = square;
			// Inverse le y de la case pour les noirs
			if (get_bit(occupancy_boards[Color::Black], square))
				table_square = (square % 8) + ((7 - (square / 8)) * 8);
			if (half_turn < 50)
				score += BitBoardGlobals::globals.middle_game_tables[i / 2][table_square] * (side_to_move == Color::White ? 1 : -1);
			else
				score += BitBoardGlobals::globals.end_game_tables[i / 2][table_square] * (side_to_move == Color::White ? 1 : -1);
			pop_bit(current_piece_board, square);
		}
		score += get_value(i) * count_bits(piece_boards[i]);
	}

	Color side_save = side_to_move;

/*	side_to_move = Color::White;
	auto white_moves = generate_moves();
	score += white_moves.size();
	side_to_move = Color::Black;
	auto black_moves = generate_moves();
	score -= black_moves.size();
	side_to_move = side_save;*/

	if (count_bits(piece_boards[Piece::White_Bishop]) >= 2)
		score += 15;
	if (count_bits(piece_boards[Piece::Black_Bishop]) >= 2)
		score -= 15;

	uint64_t king_mask = BitBoardGlobals::globals.get_king_threats_mask(get_least_significant_bit(piece_boards[Piece::White_King]));
	while (king_mask)
	{
		uint8_t square = get_least_significant_bit(king_mask);
		if (is_square_attacked(square, Color::Black))
			score -= 48;
		pop_bit(king_mask, square);
	}

	king_mask = BitBoardGlobals::globals.get_king_threats_mask(get_least_significant_bit(piece_boards[Piece::Black_King]));
	while (king_mask)
	{
		uint8_t square = get_least_significant_bit(king_mask);
		if (is_square_attacked(square, Color::White))
			score += 48;
		pop_bit(king_mask, square);
	}

	if (is_square_attacked(get_least_significant_bit(piece_boards[Piece::White_King]), Color::Black))
		score -= 48;
	if (is_square_attacked(get_least_significant_bit(piece_boards[Piece::Black_King]), Color::White))
		score += 48;

	for (uint8_t i = 0; i < 8; i++)
	{
		// Pions doublés
		uint8_t w_pawn_on_i = count_bits(piece_boards[Piece::White_Pawn] & (~BitBoardGlobals::globals.not_col[i]));
		uint8_t b_pawn_on_i = count_bits(piece_boards[Piece::Black_Pawn] & (~BitBoardGlobals::globals.not_col[i]));
		if (w_pawn_on_i > 1)
			score -= 25;
		if (b_pawn_on_i > 1)
			score += 25;

		// Pions isolés
		if ((i == a || count_bits(piece_boards[Piece::White_Pawn] & (~BitBoardGlobals::globals.not_col[i - 1])) == 0) &&
			(i == h || count_bits(piece_boards[Piece::White_Pawn] & (~BitBoardGlobals::globals.not_col[i + 1])) == 0))
			score -= 20;
		if ((i == a || count_bits(piece_boards[Piece::Black_Pawn] & (~BitBoardGlobals::globals.not_col[i - 1])) == 0) &&
			(i == h || count_bits(piece_boards[Piece::Black_Pawn] & (~BitBoardGlobals::globals.not_col[i + 1])) == 0))
			score += 20;
	}

	// Knight supported by a pawn
	uint64_t knights = piece_boards[Piece::White_Knight];
	while (knights)
	{
		uint8_t square = get_least_significant_bit(knights);
		if (BitBoardGlobals::globals.pawn_masks.find(Color::Black)->second[square] & piece_boards[Piece::White_Pawn])
			score += 6;
		pop_bit(knights, square);
	}

	knights = piece_boards[Piece::Black_Knight];
	while (knights)
	{
		uint8_t square = get_least_significant_bit(knights);
		if (BitBoardGlobals::globals.pawn_masks.find(Color::White)->second[square] & piece_boards[Piece::Black_Pawn])
			score += 6;
		pop_bit(knights, square);
	}

	return score;
}
