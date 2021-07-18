#include "BitBoard.h"

/*
**  ==============================
**
**  Move Generator
**
**  ==============================
*/

std::vector<uint16_t> BitBoard::generate_moves()
{
	std::vector<uint16_t> moves;
	moves.reserve(50);
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

				for (current_piece_attacks = BitBoardGlobals::globals.pawn_masks.find(side_to_move)->second[start] & occupancy_boards[!side_to_move]; current_piece_attacks != 0;)
				{
					uint8_t attack_target = get_least_significant_bit(current_piece_attacks);
					moves.push_back((static_cast<uint16_t>(start) << 8) | attack_target);
					pop_bit(current_piece_attacks, attack_target);
				}

				if (en_passant != No_Square && BitBoardGlobals::globals.pawn_masks.find(side_to_move)->second[start] & (1ULL << en_passant))
					moves.push_back((static_cast<uint16_t>(start) << 8) | en_passant);

				pop_bit(current_piece_board, start);
			}
		}

		if (side_to_move == Color::White && piece == Piece::White_King && !is_square_attacked(e1, Color::Black))
		{
			start = get_least_significant_bit(piece_boards[piece]);
			if ((allowed_castle & WK) && !get_bit(occupancy_boards[Color::Both], f1) && !get_bit(occupancy_boards[Color::Both], g1) && !is_square_attacked(f1, Color::Black) && !is_square_attacked(g1, Color::Black))
				moves.push_back((static_cast<uint16_t>(start) << 8) | g1);
			if (allowed_castle & WQ && !get_bit(occupancy_boards[Color::Both], d1) && !get_bit(occupancy_boards[Color::Both], c1) && !get_bit(occupancy_boards[Color::Both], b1) && !is_square_attacked(c1, Color::Black) && !is_square_attacked(d1, Color::Black))
				moves.push_back((static_cast<uint16_t>(start) << 8) | c1);
		}
		else if (side_to_move == Color::Black && piece == Piece::Black_King && !is_square_attacked(e8, Color::White))
		{
			start = get_least_significant_bit(piece_boards[piece]);
			if (allowed_castle & BK && !get_bit(occupancy_boards[Color::Both], f8) && !get_bit(occupancy_boards[Color::Both], g8) && !is_square_attacked(f8, Color::White) && !is_square_attacked(g8, Color::White))
				moves.push_back((static_cast<uint16_t>(start) << 8) | g8);
			if (allowed_castle & BQ && !get_bit(occupancy_boards[Color::Both], d8) && !get_bit(occupancy_boards[Color::Both], c8) && !get_bit(occupancy_boards[Color::Both], b8) && !is_square_attacked(c8, Color::White) && !is_square_attacked(d8, Color::White))
				moves.push_back((static_cast<uint16_t>(start) << 8) | c8);
		}

		if ((side_to_move == Color::White) ? (piece == Piece::White_Knight || piece == Piece::White_Bishop || piece == Piece::White_Rook || piece == Piece::White_Queen || piece == Piece::White_King)
				: (piece == Piece::Black_Knight || piece == Piece::Black_Bishop || piece == Piece::Black_Rook || piece == Piece::Black_Queen || piece == Piece::Black_King))
		{
			while (current_piece_board)
			{
				start = get_least_significant_bit(current_piece_board);

				if (piece == Piece::White_Knight || piece == Piece::Black_Knight)
					current_piece_attacks = BitBoardGlobals::globals.knight_masks[start] & (~occupancy_boards[side_to_move]);
				else if (piece == Piece::White_King || piece == Piece::Black_King)
					current_piece_attacks = BitBoardGlobals::globals.king_masks[start] & (~occupancy_boards[side_to_move]);
				else if (piece == Piece::White_Rook || piece == Piece::Black_Rook)
					current_piece_attacks = BitBoardGlobals::globals.get_rook_attacks(start, occupancy_boards[Color::Both]) & (~occupancy_boards[side_to_move]);
				else if (piece == Piece::White_Bishop || piece == Piece::Black_Bishop)
					current_piece_attacks = BitBoardGlobals::globals.get_bishop_attacks(start, occupancy_boards[Color::Both]) & (~occupancy_boards[side_to_move]);
				else
					current_piece_attacks = BitBoardGlobals::globals.get_queen_attacks(start, occupancy_boards[Color::Both]) & (~occupancy_boards[side_to_move]);

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

	return moves;
}

void BitBoard::handle_castling(uint16_t move)
{
	uint8_t start = (move & 0xFF00) >> 8, target = move & 0xFF;
	Color enemy_color = Color(!side_to_move);

	if (get_bit(piece_boards[Piece::White_King], start) || get_bit(piece_boards[Piece::Black_King], start))
	{
		if ((target % 8) - (start % 8) == -2)
		{
			uint8_t target_square = (side_to_move == Color::White ? d1 : d8), start_square = (side_to_move == Color::White ? a1 : a8);

			pop_bit(side_to_move == Color::White ? piece_boards[Piece::White_Rook] : piece_boards[Piece::Black_Rook], start_square);
			set_bit(side_to_move == Color::White ? piece_boards[Piece::White_Rook] : piece_boards[Piece::Black_Rook], target_square);

			pop_bit(occupancy_boards[side_to_move], start_square);
			set_bit(occupancy_boards[side_to_move], target_square);

			pop_bit(occupancy_boards[Color::Both], start_square);
			set_bit(occupancy_boards[Color::Both], target_square);
		}

		// Petit roque
		if ((target % 8) - (start % 8) == 2)
		{
			uint8_t target_square = (side_to_move == Color::White ? f1 : f8), start_square = (side_to_move == Color::White ? h1 : h8);

			pop_bit(side_to_move == Color::White ? piece_boards[Piece::White_Rook] : piece_boards[Piece::Black_Rook], start_square);
			set_bit(side_to_move == Color::White ? piece_boards[Piece::White_Rook] : piece_boards[Piece::Black_Rook], target_square);

			pop_bit(occupancy_boards[side_to_move], start_square);
			set_bit(occupancy_boards[side_to_move], target_square);

			pop_bit(occupancy_boards[Color::Both], start_square);
			set_bit(occupancy_boards[Color::Both], target_square);
		}
	}

	// Si le roi bouge il y a plus de roques
	if (piece_boards[Piece::White_King] & (1ULL << start))
		allowed_castle &= (~(WK | WQ));
	if (piece_boards[Piece::Black_King] & (1ULL << start))
		allowed_castle &= (~(BK | BQ));

	// Si une tour bouge on désactive un roque
	if (piece_boards[Piece::White_Rook] & (1ULL << start))
	{
		if (start % 8 == 0)
			allowed_castle &= ~WQ;
		else if (start % 8 == 7)
			allowed_castle &= ~WK;
	}

	if (piece_boards[Piece::Black_Rook] & (1ULL << start))
	{
		if (start % 8 == 0)
			allowed_castle &= ~BQ;
		else if (start % 8 == 7)
			allowed_castle &= ~BK;
	}

	// Si une tour se fait manger on désactive un roque
	if (target == a1)
		allowed_castle &= ~WQ;
	else if (target == h1)
		allowed_castle &= ~WK;
	else if (target == a8)
		allowed_castle &= ~BQ;
	else if (target == h8)
		allowed_castle &= ~BK;
}

void BitBoard::move_piece(uint16_t move)
{
	last_move_is_capture = false;
	// Roque
	handle_castling(move);

	// Prise en passant
	uint8_t start = (move & 0xFF00) >> 8;
	uint8_t target = move & 0xFF;

	Piece moved_piece = No_Piece;
	for (uint8_t i = 0; i < 12; i++)
	{
		if (piece_boards[i] & (1ULL << start))
		{
			moved_piece = Piece(i);
			break;
		}
	}

	if (target == en_passant && (moved_piece == Piece::White_Pawn || moved_piece == Piece::Black_Pawn))
	{
		uint8_t pop_at = en_passant + (moved_piece == Piece::White_Pawn ? 8 : -8);
		pop_bit(occupancy_boards[Color(!side_to_move)], pop_at);
		pop_bit(occupancy_boards[Color::Both], pop_at);
		for (uint8_t i = !side_to_move; i < 12; i+=2)
			pop_bit(piece_boards[i], pop_at);
		last_move_is_capture = true;
	}

	if ((moved_piece == Piece::White_Pawn || moved_piece == Piece::Black_Pawn) && abs(target - start) == 16)
		en_passant = target % 8 + ((start / 8 + (target / 8 - start / 8) / 2) * 8);
	else
		en_passant = No_Square;

	// Déplacement de la pièce
	set_bit(piece_boards[moved_piece], target);
	set_bit(occupancy_boards[Color::Both], target);
	set_bit(occupancy_boards[side_to_move], target);
	if (get_bit(occupancy_boards[!side_to_move], target))
	{
		last_move_is_capture = true;
		pop_bit(occupancy_boards[!side_to_move], target);
		for (uint8_t i = !side_to_move; i < 12; i+=2)
			pop_bit(piece_boards[i], target);
	}
	pop_bit(piece_boards[moved_piece], start);
	pop_bit(occupancy_boards[Color::Both], start);
	pop_bit(occupancy_boards[side_to_move], start);

	// Promotion
	if ((moved_piece == Piece::White_Pawn || moved_piece == Piece::Black_Pawn) && (target / 8 == 7 || target / 8 == 0))
	{
		pop_bit(piece_boards[moved_piece], target);
		set_bit(piece_boards[Piece::White_Queen + side_to_move], target);
	}

	last_move = move;
	half_turn++;
	side_to_move = (side_to_move == Color::White ? Color::Black : Color::White);
}
