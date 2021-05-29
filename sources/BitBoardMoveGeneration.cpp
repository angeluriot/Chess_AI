#include "BitBoard.h"

/*
**  ==============================
**
**  Move Generator
**
**  ==============================
*/

std::list<uint16_t> BitBoard::generate_moves(const BitBoardGlobals& globals)
{
	std::list<uint16_t> moves;
	uint64_t current_piece_board, current_piece_attacks;
	uint8_t start, target;

	for (uint8_t piece = 0; piece < 12; piece++)
	{
		current_piece_board = piece_boards[piece];

		if (((side_to_move == Color::White) ? (piece == Piece::White_Pawn) : (piece == Piece::Black_Pawn)))
		{
			int8_t dir = (side_to_move == Color::White ? -8 : 8);
			while (current_piece_board != 0)
			{
				start = get_least_significant_bit(current_piece_board);
				target = start + dir;

				if (start >= 8 && start <= 55 && !get_bit(occupancy_boards[Color::Both], target))
				{
					moves.push_back((static_cast<uint16_t>(start) << 8) | target);

					if (start / 8 == (side_to_move == Color::White ? 6 : 1) && !get_bit(occupancy_boards[Color::Both], target + dir))
						moves.push_back((static_cast<uint16_t>(start) << 8) | (target + dir));
				}

				for (current_piece_attacks = globals.pawn_masks.find(side_to_move)->second[start] & occupancy_boards[!side_to_move]; current_piece_attacks != 0;)
				{
					uint8_t attack_target = get_least_significant_bit(current_piece_attacks);
					moves.push_back((static_cast<uint16_t>(start) << 8) | attack_target);
					pop_bit(current_piece_attacks, attack_target);
				}

				if (en_passant != No_Square && globals.pawn_masks.find(side_to_move)->second[start] & (1ULL << en_passant))
					moves.push_back((static_cast<uint16_t>(start) << 8) | en_passant);

				pop_bit(current_piece_board, start);
			}
		}

		if (side_to_move == Color::White && piece == Piece::White_King)
		{
			if ((allowed_castle & WK) && !get_bit(occupancy_boards[Color::Both], f1) && !get_bit(occupancy_boards[Color::Both], g1) && !is_square_attacked(globals, f1, Color::Black) && !is_square_attacked(globals, g1, Color::Black))
				moves.push_back((static_cast<uint16_t>(start) << 8) | g1);
			if (allowed_castle & WQ && !get_bit(occupancy_boards[Color::Both], d1) && !get_bit(occupancy_boards[Color::Both], c1) && !get_bit(occupancy_boards[Color::Both], b1) && !is_square_attacked(globals, c1, Color::Black) && !is_square_attacked(globals, d1, Color::Black))
				moves.push_back((static_cast<uint16_t>(start) << 8) | c1);
		}
		else if (side_to_move == Color::Black && piece == Piece::Black_King)
		{
			if (allowed_castle & BK && !get_bit(occupancy_boards[Color::Both], f8) && !get_bit(occupancy_boards[Color::Both], g8) && !is_square_attacked(globals, f8, Color::White) && !is_square_attacked(globals, g8, Color::White))
				moves.push_back((static_cast<uint16_t>(start) << 8) | g8);
			if (allowed_castle & BQ && !get_bit(occupancy_boards[Color::Both], d8) && !get_bit(occupancy_boards[Color::Both], c8) && !get_bit(occupancy_boards[Color::Both], b8) && !is_square_attacked(globals, c8, Color::White) && !is_square_attacked(globals, d8, Color::White))
				moves.push_back((static_cast<uint16_t>(start) << 8) | c8);
		}

		if ((side_to_move == Color::White) ? (piece == Piece::White_Knight || piece == Piece::White_Bishop || piece == Piece::White_Rook || piece == Piece::White_Queen || piece == Piece::White_King)
				: (piece == Piece::Black_Knight || piece == Piece::Black_Bishop || piece == Piece::Black_Rook || piece == Piece::Black_Queen || piece == Piece::Black_King))
		{
			while (current_piece_board)
			{
				start = get_least_significant_bit(current_piece_board);

				if (piece == Piece::White_Knight || piece == Piece::Black_Knight)
					current_piece_attacks = globals.knight_masks[start] & (~occupancy_boards[side_to_move]);
				else if (piece == Piece::White_King || piece == Piece::Black_King)
					current_piece_attacks = globals.king_masks[start] & (~occupancy_boards[side_to_move]);
				else if (piece == Piece::White_Rook || piece == Piece::Black_Rook)
					current_piece_attacks = globals.get_rook_attacks(start, occupancy_boards[Color::Both]) & (~occupancy_boards[side_to_move]);
				else if (piece == Piece::White_Bishop || piece == Piece::Black_Bishop)
					current_piece_attacks = globals.get_bishop_attacks(start, occupancy_boards[Color::Both]) & (~occupancy_boards[side_to_move]);
				else
					current_piece_attacks = globals.get_queen_attacks(start, occupancy_boards[Color::Both]) & (~occupancy_boards[side_to_move]);

				while (current_piece_attacks)
				{
					target = get_least_significant_bit(current_piece_attacks);
					moves.push_back((static_cast<uint16_t>(start) << 8) | target);
					pop_bit(current_piece_attacks, target);
				}

				pop_bit(current_piece_board, start);
			}
		}
	}

	std::cout << moves.size() << std::endl;
	return moves;
}
