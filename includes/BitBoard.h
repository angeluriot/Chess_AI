#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <string>
#include <array>
#include <iostream>
#include "utils.h"
#include <unordered_map>

/*
**  ==============================
**
**  Enums
**
**  ==============================
*/

enum : uint8_t {
	a8, b8, c8, d8, e8, f8, g8, h8,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a1, b1, c1, d1, e1, f1, g1, h1,
	No_Square
};

enum Piece : uint8_t
{
	White_Pawn = 0, White_Knight = 2, White_Bishop = 4, White_Rook = 6, White_Queen = 8, White_King = 10,
	Black_Pawn = 1, Black_Knight = 3, Black_Bishop = 5, Black_Rook = 7, Black_Queen = 9, Black_King = 11,
	No_Piece = 12
};

enum Color : int8_t
{
	White,
	Black,
	Both,
	No_Color
};

enum : uint8_t { a, b, c, d, e, f, g, h };

enum Castle : uint8_t
{
	WK = 1, WQ = 2, BK = 4, BQ = 8
};

/*
**  ==============================
**
**  Globals
**
**  ==============================
*/

struct BitBoardGlobals
{
	std::array<std::string, 64> square_to_coords;
	std::array<uint64_t, 8> not_col;
	std::array<uint64_t, 8> not_rank;
	std::array<uint8_t, 64> rook_relevant_bits;
	std::array<uint8_t, 64> bishop_relevant_bits;
	std::array<uint64_t, 64> rook_magic_numbers;
	std::array<uint64_t, 64> bishop_magic_numbers;
	std::unordered_map<Color, std::array<uint64_t, 64>> pawn_masks;
	std::array<uint64_t, 64> knight_masks, king_masks, bishop_masks, rook_masks;
	std::array<std::array<uint64_t, 4096>, 64> rook_attacks;
	std::array<std::array<uint64_t, 512>, 64> bishop_attacks;
	std::array<std::array<int, 64>, 12> middle_game_tables;
	std::array<std::array<int, 64>, 12> end_game_tables;
	std::array<uint64_t, 781> zobrist_keys;

	BitBoardGlobals();

	inline uint64_t get_pawn_threats_mask(uint8_t square, Color color) const
	{
		return (color == Color::White ? (((1ULL << square) >> 7) & not_col[a]) | (((1ULL << square) >> 9) & not_col[h]) : (((1ULL << square) << 7) & not_col[h]) | (((1ULL << square) << 9) & not_col[a]));
	}

	inline uint64_t get_bishop_attacks(uint8_t square, uint64_t occupancy) const
	{
		return bishop_attacks[square][(((occupancy & bishop_masks[square]) * bishop_magic_numbers[square]) >> (64 - bishop_relevant_bits[square]))];
	}

	inline uint64_t get_rook_attacks(uint8_t square, uint64_t occupancy) const
	{
		return rook_attacks[square][(((occupancy & rook_masks[square]) * rook_magic_numbers[square]) >> (64 - rook_relevant_bits[square]))];
	}

	inline uint64_t get_queen_attacks(uint8_t square, uint64_t occupancy) const
	{
		return get_rook_attacks(square, occupancy) | get_bishop_attacks(square, occupancy);
	}

	inline uint64_t magic_number_candidate() const
	{
		return random_64() & random_64() & random_64();
	}

	uint64_t get_knight_threats_mask(uint8_t square) const;
	uint64_t get_king_threats_mask(uint8_t square) const;
	uint64_t get_bishop_threats_mask(uint8_t square) const;
	uint64_t get_rook_threats_mask(uint8_t square) const;
	uint64_t bishop_threats_with_blockers(uint8_t square, uint64_t blockers) const;
	uint64_t rook_threats_with_blockers(uint8_t square, uint64_t blockers) const;
	uint64_t set_occupancy(uint64_t index, uint8_t bits_in_mask, uint64_t attack_mask) const;
	uint64_t get_key(uint8_t piece, uint8_t square) const;


	private:
		void init_sliders_attacks(Piece piece);
		uint64_t find_magic_number(uint8_t square, uint8_t relevant, bool is_rook) const;

	public:
		static BitBoardGlobals globals;
};

inline bool get_bit(const uint64_t& board, uint8_t bit) { return (board & (1ULL << bit)) ? true : false; }
inline void set_bit(uint64_t& board, uint8_t bit) { board |= (1ULL << bit); }
inline void pop_bit(uint64_t& board, uint8_t bit) { get_bit(board, bit) ? (board ^= (1ULL << bit)) : 0; }
void print_bitboard(uint64_t board);
uint8_t count_bits(uint64_t board);
uint8_t get_least_significant_bit(uint64_t board);

class BitBoard
{
public:
	std::array<uint64_t, 12> piece_boards;
	std::array<uint64_t, 3> occupancy_boards;
	Color side_to_move;
	uint8_t allowed_castle;
	uint8_t en_passant;
	uint16_t half_turn;
	uint8_t clicked_cell;
	uint16_t last_move;
	bool last_move_is_capture;

	BitBoard() = default;
	BitBoard(const BitBoard& other);
	BitBoard(const std::string& fen);
	void operator=(const BitBoard& other);

	void draw_pieces(sf::RenderWindow& window, std::map<Piece, sf::Texture>& textures, float cell_size);
	std::vector<uint16_t> generate_moves();
	bool is_square_attacked(uint8_t square, Color color);
	void move_piece(uint16_t move);
	void handle_castling(uint16_t move);
	void check_click_on_piece(const sf::RenderWindow& window, float cell_size, BitBoard* last_board = nullptr);
	void draw_last_move(sf::RenderWindow& window, std::map<Piece, sf::Texture>& textures, float cell_size);
	bool is_finished() const;
	BitBoard get_moved_board(uint16_t move) const;
	uint64_t signature_hash();
	uint8_t piece_at(uint8_t square) const;
	int get_score();
};

#endif
