#include "BitBoard.h"

BitBoardGlobals BitBoardGlobals::globals = BitBoardGlobals();

/*
**  ==============================
**
**  BitBoard
**
**  ==============================
*/

BitBoard::BitBoard(const BitBoard& other)
{
	piece_boards = other.piece_boards;
	occupancy_boards = other.occupancy_boards;
	side_to_move = other.side_to_move;
	allowed_castle = other.allowed_castle;
	en_passant = other.en_passant;
	half_turn = other.half_turn;
	clicked_cell = other.clicked_cell;
	last_move = other.last_move;
	last_move_is_capture = other.last_move_is_capture;
}

void BitBoard::operator=(const BitBoard& other)
{
	piece_boards = other.piece_boards;
	occupancy_boards = other.occupancy_boards;
	side_to_move = other.side_to_move;
	allowed_castle = other.allowed_castle;
	en_passant = other.en_passant;
	half_turn = other.half_turn;
	clicked_cell = other.clicked_cell;
	last_move = other.last_move;
	last_move_is_capture = other.last_move_is_capture;
}


static Piece get_piece_from_fen(char c)
{
	char c2 = std::toupper(c);
	Piece ret = Piece::No_Piece;

	switch (c2)
	{
		case 'R':
			ret = Piece::White_Rook; break;
		case 'N':
			ret = Piece::White_Knight; break;
		case 'B':
			ret = Piece::White_Bishop; break;
		case 'Q':
			ret = Piece::White_Queen; break;
		case 'K':
			ret = Piece::White_King; break;
		case 'P':
			ret = Piece::White_Pawn; break;
	}
	return Piece(ret + !std::isupper(c));
}

BitBoard::BitBoard(const std::string& fen) : half_turn(0), clicked_cell(No_Square), last_move(No_Square), en_passant(No_Square)
{
	for (auto& board : occupancy_boards)
		board = 0;
	for (auto& board : piece_boards)
		board = 0;


	// Lecture du fen pour placer les pièces
	uint8_t x = 0, y = 0;
	size_t i = 0;

	for (; fen[i] && fen[i] != ' '; i++)
	{
		if (fen[i] == '/')
		{
			y++;
			x = 0;
		}
		else if (std::isdigit(fen[i]))
			x += (fen[i] - '0');
		else
		{
			Piece type = get_piece_from_fen(fen[i]);
			set_bit(piece_boards[type], y * 8 + x);
			set_bit(occupancy_boards[Color::Both], y * 8 + x);
			set_bit(occupancy_boards[type % 2], y * 8 + x);
			x++;
		}
	}
	i++;

	side_to_move = (fen[i] == 'w' ? Color::White : Color::Black);

	i+=2;
	while (fen[i] != ' ')
	{
		switch (fen[i])
		{
			case 'K':
				allowed_castle |= WK; break;
			case 'Q':
				allowed_castle |= WQ; break;
			case 'k':
				allowed_castle |= BK; break;
			case 'q':
				allowed_castle |= BQ; break;
		}
		i++;
	}
	i++;

	if (fen[i] != '-')
	{
		uint8_t x = fen[i] - 'a';
		uint8_t y = 8 - (fen[i + 1] - '0');
		en_passant = y * 8 + x;
		i += 2;
	}
	else
		i++;
}

/*
**  ==============================
**
**  Utils
**
**  ==============================
*/

BitBoard BitBoard::get_moved_board(uint16_t move) const
{
	BitBoard cpy(*this);
	cpy.move_piece(move);
	return cpy;
}

bool BitBoard::is_finished() const
{
	return count_bits(piece_boards[Piece::White_King]) == 0 || count_bits(piece_boards[Piece::Black_King]) == 0;
}

bool BitBoard::is_square_attacked(uint8_t square, Color color)
{
	if (color == Color::White && piece_boards[Piece::White_Pawn] & BitBoardGlobals::globals.pawn_masks.find(Color::Black)->second[square])
		return true;
	if (color == Color::Black && piece_boards[Piece::Black_Pawn] & BitBoardGlobals::globals.pawn_masks.find(Color::White)->second[square])
		return true;
	if (BitBoardGlobals::globals.knight_masks[square] & (color == Color::White ? piece_boards[Piece::White_Knight] : piece_boards[Piece::Black_Knight]))
		return true;
	if (BitBoardGlobals::globals.king_masks[square] & (color == Color::White ? piece_boards[Piece::White_King] : piece_boards[Piece::Black_King]))
		return true;
	if (BitBoardGlobals::globals.get_bishop_attacks(square, occupancy_boards[Color::Both]) & (color == Color::White ? piece_boards[Piece::White_Bishop] : piece_boards[Piece::Black_Bishop]))
		return true;
	if (BitBoardGlobals::globals.get_rook_attacks(square, occupancy_boards[Color::Both]) & (color == Color::White ? piece_boards[Piece::White_Rook] : piece_boards[Piece::Black_Rook]))
		return true;
	if (BitBoardGlobals::globals.get_queen_attacks(square, occupancy_boards[Color::Both]) & (color == Color::White ? piece_boards[Piece::White_Queen] : piece_boards[Piece::Black_Queen]))
		return true;
	return false;
}

uint64_t BitBoard::signature_hash()
{
	uint64_t ret = 0;

	for (uint8_t i = 0; i < 12; i++)
	{
		uint64_t board = piece_boards[i];
		while (board)
		{
			uint8_t square = get_least_significant_bit(board);
			ret ^= BitBoardGlobals::globals.get_key(i, square);
			pop_bit(board, square);
		}
	}

	if (allowed_castle & WK)
		ret ^= BitBoardGlobals::globals.zobrist_keys[64 * 12 + 0];
	if (allowed_castle & WQ)
		ret ^= BitBoardGlobals::globals.zobrist_keys[64 * 12 + 1];
	if (allowed_castle & BK)
		ret ^= BitBoardGlobals::globals.zobrist_keys[64 * 12 + 2];
	if (allowed_castle & BQ)
		ret ^= BitBoardGlobals::globals.zobrist_keys[64 * 12 + 3];

	if (en_passant != No_Square && BitBoardGlobals::globals.pawn_masks.find(Color(!side_to_move))->second[en_passant] & piece_boards[side_to_move == Color::White ? Piece::White_Pawn : Piece::Black_Pawn])
		ret ^= BitBoardGlobals::globals.zobrist_keys[64 * 12 + 4 + (en_passant % 8)];

	if (side_to_move == Color::White)
		ret ^= BitBoardGlobals::globals.zobrist_keys[64 * 12 + 4 + 8];
	return ret;
}

uint8_t BitBoard::piece_at(uint8_t square) const
{
	if (!get_bit(occupancy_boards[Color::Both], square))
		return No_Piece;
	for (uint8_t i = (get_bit(occupancy_boards[Color::White], square) ? 0 : 1); i < 12; i+=2)
		if (get_bit(piece_boards[i], square))
			return i;
	return No_Piece;
}

/*
**  ==============================
**
**  UI
**
**  ==============================
*/

void BitBoard::draw_pieces(sf::RenderWindow& window, std::map<Piece, sf::Texture>& textures, float cell_size)
{
	sf::RenderTexture tex;

	tex.create(cell_size * 8, cell_size * 8);

	uint8_t type = 0;
	for (auto& board : piece_boards)
	{
		for (uint8_t square = 0; square < 64; square++)
		{
			if (get_bit(board, square))
			{
				Piece piece = Piece(type);
				sf::Sprite sprite = sf::Sprite(textures[piece]);
				sprite.setOrigin({ textures[piece].getSize().x / 2.f, textures[piece].getSize().y / 2.f});
				sprite.setScale(cell_size / static_cast<float>(textures[piece].getSize().x), cell_size / static_cast<float>(textures[piece].getSize().y) * -1);
				sprite.setPosition({cell_size * (square % 8) + (cell_size / 2.f), cell_size * (7 - (square / 8)) + (cell_size / 2)});
				tex.draw(sprite);
			}
		}
		type++;
	}

	sf::Sprite tex_spr(tex.getTexture());
	tex_spr.setPosition((window.getSize().x - tex.getSize().x) / 2.f, 0);
	window.draw(tex_spr);
}

void BitBoard::check_click_on_piece(const sf::RenderWindow& window, float cell_size, BitBoard* last_board)
{
	static bool clicked_last_frame = false;

	if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		clicked_last_frame = false;
		return;
	}
	if (clicked_last_frame)
		return;
	clicked_last_frame = true;

	if ((sf::Mouse::getPosition(window).x - (window.getSize().x / 2.f) + (cell_size * 4)) < 0 || sf::Mouse::getPosition(window).y < 0)
		return;

	uint8_t cell_pos = static_cast<int8_t>((sf::Mouse::getPosition(window).x - (window.getSize().x / 2.f) + (cell_size * 4)) / cell_size) + static_cast<int8_t>(sf::Mouse::getPosition(window).y / cell_size) * 8;

	if (cell_pos < 0 || cell_pos > 63)
		return;


	if (clicked_cell != No_Square)
	{
		auto moves = generate_moves();
		auto move_it = std::find(moves.begin(), moves.end(), (clicked_cell << 8) | cell_pos);
		if (move_it != moves.end())
		{
			*last_board = *this;
			move_piece(*move_it);
		}
		clicked_cell = No_Square;
		return;
	}

	if (get_bit(occupancy_boards[side_to_move], cell_pos))
		clicked_cell = cell_pos;
}

void BitBoard::draw_last_move(sf::RenderWindow& window, std::map<Piece, sf::Texture>& textures, float cell_size)
{
	sf::RenderTexture tex;
	sf::RectangleShape cell;

	if (last_move == No_Square)
		return;

	uint8_t start = (last_move & 0xFF00) >> 8, target = last_move & 0xFF;

	tex.create(cell_size * 8, cell_size * 8);

	cell.setSize({cell_size, cell_size});
	cell.setFillColor(sf::Color(255, 255, 0, 150));

	cell.setPosition(sf::Vector2f((target % 8) * cell_size, (7 - target / 8) * cell_size));
	tex.draw(cell);
	cell.setPosition(sf::Vector2f((start % 8) * cell_size, (7 - start / 8) * cell_size));
	tex.draw(cell);

	sf::Sprite sprite(tex.getTexture());
	sprite.setPosition({(window.getSize().x - tex.getSize().x) / 2.f, (window.getSize().y - tex.getSize().y) / 2.f});
	window.draw(sprite);
}
