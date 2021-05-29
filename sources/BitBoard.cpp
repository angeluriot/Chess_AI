#include "BitBoard.h"

/*
**  ==============================
**
**  BitBoard
**
**  ==============================
*/


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

BitBoard::BitBoard(const std::string& fen)
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

bool BitBoard::is_square_attacked(const BitBoardGlobals& globals, uint8_t square, Color color)
{
	if (color == Color::White && piece_boards[Piece::White_Pawn] & globals.pawn_masks.find(Color::Black)->second[square])
		return true;
	if (color == Color::Black && piece_boards[Piece::Black_Pawn] & globals.pawn_masks.find(Color::White)->second[square])
		return true;
	if (globals.knight_masks[square] & (color == Color::White ? piece_boards[Piece::White_Knight] : piece_boards[Piece::Black_Knight]))
		return true;
	if (globals.king_masks[square] & (color == Color::White ? piece_boards[Piece::White_King] : piece_boards[Piece::Black_King]))
		return true;
	if (globals.get_bishop_attacks(square, occupancy_boards[Color::Both]) & (color == Color::White ? piece_boards[Piece::White_Bishop] : piece_boards[Piece::Black_Bishop]))
		return true;
	if (globals.get_rook_attacks(square, occupancy_boards[Color::Both]) & (color == Color::White ? piece_boards[Piece::White_Rook] : piece_boards[Piece::Black_Rook]))
		return true;
	if (globals.get_queen_attacks(square, occupancy_boards[Color::Both]) & (color == Color::White ? piece_boards[Piece::White_Queen] : piece_boards[Piece::Black_Queen]))
		return true;
	return false;
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
