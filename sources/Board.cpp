#include "Board.h"

Board::Board()
{
	for (uint8_t i = 0; i < 2; i++)
		std::fill(board[i].begin(), board[i].end(), out_of_range);

	for (uint8_t i = 10; i < 12; i++)
		std::fill(board[i].begin(), board[i].end(), out_of_range);

	for (uint8_t i = 2; i < 10; i++)
		for (uint8_t j = 0; j < 12; j++)
		{
			if (j > 1) j = 10;
			board[i][j] = out_of_range;
		}

	for (uint8_t i = 2; i < 10; i++)
		for (uint8_t j = 4; j < 8; j++)
			board[i][j] = no_piece;

	for (uint8_t i = 2; i < 10; i++)
	{
		board[i][3] = Black_pawn;
		board[i][8] = White_pawn;
	}

	board[2][2] = Black_rook;
	board[3][2] = Black_knight;
	board[4][2] = Black_bishop;
	board[5][2] = Black_queen;
	board[6][2] = Black_king;
	board[7][2] = Black_bishop;
	board[8][2] = Black_knight;
	board[9][2] = Black_rook;

	board[2][9] = White_rook;
	board[3][9] = White_knight;
	board[4][9] = White_bishop;
	board[5][9] = White_queen;
	board[6][9] = White_king;
	board[7][9] = White_bishop;
	board[8][9] = White_knight;
	board[9][9] = White_rook;

	sf::Texture white_pawn; white_pawn.loadFromFile("dependencies/resources/white_pawn.png");
	sf::Texture white_rook; white_rook.loadFromFile("dependencies/resources/white_rook.png");
	sf::Texture white_knight; white_knight.loadFromFile("dependencies/resources/white_knight.png");
	sf::Texture white_bishop; white_bishop.loadFromFile("dependencies/resources/white_bishop.png");
	sf::Texture white_queen; white_queen.loadFromFile("dependencies/resources/white_queen.png");
	sf::Texture white_king; white_king.loadFromFile("dependencies/resources/white_king.png");
	sf::Texture black_pawn; black_pawn.loadFromFile("dependencies/resources/black_pawn.png");
	sf::Texture black_rook; black_rook.loadFromFile("dependencies/resources/black_rook.png");
	sf::Texture black_knight; black_knight.loadFromFile("dependencies/resources/black_knight.png");
	sf::Texture black_bishop; black_bishop.loadFromFile("dependencies/resources/black_bishop.png");
	sf::Texture black_queen; black_queen.loadFromFile("dependencies/resources/black_queen.png");
	sf::Texture black_king; black_king.loadFromFile("dependencies/resources/black_king.png");

	std::map<Piece_type, sf::Texture> sprites = {
		{ Black_pawn, black_pawn },
		{ Black_rook, black_rook },
		{ Black_knight, black_knight },
		{ Black_bishop, black_bishop },
		{ Black_queen, black_queen },
		{ Black_king, black_king },
		{ White_pawn, white_pawn },
		{ White_rook, white_rook },
		{ White_knight, white_knight },
		{ White_bishop, white_bishop },
		{ White_queen, white_queen },
		{ White_king, white_king }
	};

	en_passant = Position::invalid;
	player_turn = PieceColor::White;

	clear_moves();
	generate_moves(White);
}

Board::Board(const Board& other)
{
	*this = other;
}

void Board::operator=(const Board& other)
{
	board = other.board;
	white_moves = other.white_moves;
	black_moves = other.black_moves;
	en_passant = other.en_passant;
	player_turn = other.player_turn;
}

Piece_type& Board::operator[](Position position)
{
	return board[position.x][position.y];
}

uint16_t Board::get_value(Piece_type piece)
{
	switch (abs(piece))
	{
	case White_pawn:
		return 1;

	case White_knight:
		return 3;

	case White_bishop:
		return 3;

	case White_rook:
		return 5;

	case White_queen:
		return 9;

	case White_king:
		return 1000;

	default:
		return 0;
	}
}

PieceColor Board::get_color(Piece_type piece)
{
	return (piece > 0 ? White : Black);
}

int16_t Board::get_score(PieceColor color)
{
	uint16_t score = 0;

	for (uint8_t i = 2; i < 10; i++)
		for (uint8_t j = 2; j < 10; j++)
			score += get_value(board[i][j]);

	return score * color;
}

int16_t Board::move_score(const Move& move, PieceColor color)
{
	return (get_color(board[move.target.x][move.target.y]) == color ? -get_value(board[move.target.x][move.target.y]) : get_value(board[move.target.x][move.target.y]));
}

void Board::clear_moves()
{
	white_moves.clear();
	black_moves.clear();
}

void Board::move_piece(Move move)
{
	if (abs(board[move.start.x][move.start.y]) == White_pawn && abs(move.target.y - move.start.y) > 1)
		en_passant = Position(move.start.x, move.start.y + (move.target.y - move.start.y) / 2);

	// Roque

	// Prise en passant

	board[move.target.x][move.target.y] = board[move.start.x][move.start.y];
	board[move.start.x][move.start.y] = no_piece;

	player_turn = PieceColor(-1 * player_turn);

	clear_moves();
}
/*
void Board::draw_pieces(sf::RenderWindow& window, float cell_size)
{
	sf::RenderTexture tex;

	tex.create(cell_size * 8, cell_size * 8);
	for (auto& piece : pieces)
	{
		sf::Sprite sprite = sf::Sprite(sprites[piece.color][piece.type->type]);
		sprite.setOrigin({ sprites[piece.color][piece.type->type].getSize().x / 2.f, sprites[piece.color][piece.type->type].getSize().y / 2.f});
		sprite.setScale(cell_size / static_cast<float>(sprites[piece.color][piece.type->type].getSize().x), cell_size / static_cast<float>(sprites[piece.color][piece.type->type].getSize().y) * -1);
		sprite.setPosition({cell_size * piece.pos.x + (cell_size / 2.f), cell_size * (7 - piece.pos.y) + (cell_size / 2)});
		tex.draw(sprite);
	}
	sf::Sprite tex_spr(tex.getTexture());
	tex_spr.setPosition((window.getSize().x - tex.getSize().x) / 2.f, 0);
	window.draw(tex_spr);
}

void Board::generate_moves(PieceColor color)
{
	for (auto& piece : pieces)
		if (piece.color == color)
			piece.generateMoves();
}

void Board::draw_moves(sf::RenderWindow& window, float cell_size)
{
	sf::RectangleShape cell;
	sf::RenderTexture tex;

	tex.create(cell_size * 8, cell_size * 8);

	cell.setSize({cell_size, cell_size});
	cell.setFillColor(sf::Color(255, 0, 0, 100));

	for (auto& piece : pieces)
	{
		if (piece.color != player_turn)
			continue;
		for (auto& move : piece.moves)
		{
			cell.setPosition({move.target.x * cell_size, (7 - move.target.y) * cell_size });
			tex.draw(cell);
		}
	}
	sf::Sprite sprite(tex.getTexture());
	sprite.setPosition({(window.getSize().x - tex.getSize().x) / 2.f, (window.getSize().y - tex.getSize().y) / 2.f});
	window.draw(sprite);
}

void Board::check_click_on_piece(const sf::RenderWindow& window, float cell_size)
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
	Position cell_pos =
	{
		static_cast<int8_t>((sf::Mouse::getPosition(window).x - (window.getSize().x / 2.f) + (cell_size * 4)) / cell_size),
		static_cast<int8_t>(sf::Mouse::getPosition(window).y / cell_size)
	};
	if (!cell_pos.is_valid())
		return;

	if (clicked_cell != Position::invalid)
	{
		Piece* piece = (*this)[clicked_cell];
		bool moved = false;
		for (auto& move : piece->moves)
		{
			if (move.target == cell_pos)
			{
				move_piece(move);
				moved = true;
				break;
			}
		}
		clicked_cell = Position::invalid;

		for (int i = 0; i < board.size(); i++)
		{
			for (int j = 0; j < board[i].size(); j++)
				std::cout << (board[j][i] ? "o" : ".");
			std::cout << std::endl;
		}
		std::cout << std::endl;

		return;
	}

	if ((*this)[cell_pos] && (*this)[cell_pos]->color == player_turn)
		clicked_cell = cell_pos;
}
*/