#include "Board.h"

Board::Board()
{
	// Ajoute les out_of_range sur les bordures
	for (uint8_t i = 0; i < 2; i++)
		std::fill(board[i].begin(), board[i].end(), Piece_type::Out_of_range);

	for (uint8_t i = 10; i < 12; i++)
		std::fill(board[i].begin(), board[i].end(), Piece_type::Out_of_range);

	for (uint8_t i = 2; i < 10; i++)
		for (uint8_t j = 0; j < 12; j++)
		{
			// La boucle fait 0 - 1 - 10 - 11
			if (j == 2) j = 10;
			board[i][j] = Piece_type::Out_of_range;
		}

	// On met des cases vides partout sur les cases valides
	for (uint8_t i = 2; i < 10; i++)
		for (uint8_t j = 4; j < 8; j++)
			board[i][j] = Piece_type::No_piece;

	// Placement des pions
	for (uint8_t i = 2; i < 10; i++)
	{
		board[i][3] = Black_pawn;
		board[i][8] = White_pawn;
	}

	// Placement des pieces
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

	en_passant = Position::invalid;
	player_turn = Color::White;

	clear_moves();

	allowed_castle = {
		{ Color::White, {true, true} },
		{ Color::Black, {true, true} }
	};

	clicked_cell = Position::invalid;
}

Board::Board(const Board& other)
{
	board = other.board;
	clear_moves();
	en_passant = other.en_passant;
	player_turn = other.player_turn;
	allowed_castle = other.allowed_castle;
}

void Board::operator=(const Board& other)
{
	board = other.board;
	white_moves = other.white_moves;
	black_moves = other.black_moves;
	en_passant = other.en_passant;
	player_turn = other.player_turn;
	allowed_castle = other.allowed_castle;
	clicked_cell = other.clicked_cell;
}

Piece_type& Board::operator[](Position position)
{
	return board[position.x][position.y];
}

uint16_t Board::get_value(Piece_type piece)
{
	switch (abs(piece))
	{
	case Piece_type::White_pawn:
		return 1;
	case Piece_type::White_knight:
		return 3;
	case Piece_type::White_bishop:
		return 3;
	case Piece_type::White_rook:
		return 5;
	case Piece_type::White_queen:
		return 9;
	case Piece_type::White_king:
		return 1000;
	default:
		return 0;
	}
}

Color Board::get_color(Piece_type piece)
{
	if (piece == Piece_type::Out_of_range || piece == Piece_type::No_piece)
		return Color::Empty;
	return (piece > 0 ? Color::White : Color::Black);
}

int16_t Board::get_score(Color color)
{
	uint16_t score = 0;

	for (uint8_t i = 2; i < 10; i++)
		for (uint8_t j = 2; j < 10; j++)
			score += get_value(board[i][j]);

	return score * color;
}

int16_t Board::move_score(const Move& move, Color color)
{
	return (get_color(board[move.target.x][move.target.y]) == color ? -get_value(board[move.target.x][move.target.y]) : get_value(board[move.target.x][move.target.y]));
}

void Board::clear_moves()
{
	white_moves.clear();
	black_moves.clear();
}

void Board::handle_castling(const Move& move)
{
	Piece_type piece = board[move.start.x][move.start.y];
	int8_t abs = std::abs(piece);
	Color enemy_color = get_color(piece) == Color::White ? Color::Black : Color::White;

	if (abs == Piece_type::White_king)
	{
		// Grand roque
		if (move.target.x - move.start.x == -2)
		{
			board[5][move.start.y] = board[2][move.start.y];
			board[2][move.start.y] = Piece_type::No_piece;
		}

		// Petit roque
		if (move.target.x - move.start.x == 2)
		{
			board[7][move.start.y] = board[9][move.start.y];
			board[9][move.start.y] = Piece_type::No_piece;
		}
	}

	// Si le roi bouge il y a plus de roques
	if (abs == Piece_type::White_king)
		allowed_castle[get_color(piece)] = { false, false };

	// Si une tour bouge on désactive un roque
	if (abs == Piece_type::White_rook && (move.start.x == 2 || move.start.x == 9))
		allowed_castle[get_color(piece)][move.start.x == 2 ? 0 : 1] = false;

	// Si une tour se fait manger on désactive un roque
	if (move.target == Position(2, 2) || move.target == Position(2, 9) || move.target == Position(9, 2) || move.target == Position(9, 9))
		allowed_castle[enemy_color][move.target.x == 2 ? 0 : 1] = false;
}

void Board::move_piece(const Move& move)
{
	// Roque
	handle_castling(move);

	// Prise en passant
	if (move.target == en_passant && std::abs(board[move.start.x][move.start.y]) == Piece_type::White_pawn)
	{
		board[en_passant.x][en_passant.y + get_color(board[move.start.x][move.start.y])] = Piece_type::No_piece;
	}

	if (abs(board[move.start.x][move.start.y]) == Piece_type::White_pawn && abs(move.target.y - move.start.y) > 1)
		en_passant = Position(move.start.x, move.start.y + (move.target.y - move.start.y) / 2);
	else
		en_passant = Position::invalid;

	board[move.target.x][move.target.y] = board[move.start.x][move.start.y];
	board[move.start.x][move.start.y] = Piece_type::No_piece;

	clear_moves();
}

Board& Board::get_moved_board(const Move& move)
{
	Board result(*this);
	result.move_piece(move);
	return result;
}

void Board::draw_pieces(sf::RenderWindow& window, std::map<Piece_type, sf::Texture>& textures, float cell_size)
{
	sf::RenderTexture tex;

	tex.create(cell_size * 8, cell_size * 8);
	for (uint8_t x = 2; x < board.size() - 2; x++)
	{
		for (uint8_t y = 2; y < board[x].size() - 2; y++)
		{
			sf::Sprite sprite = sf::Sprite(textures[board[x][y]]);
			sprite.setOrigin({ textures[board[x][y]].getSize().x / 2.f, textures[board[x][y]].getSize().y / 2.f});
			sprite.setScale(cell_size / static_cast<float>(textures[board[x][y]].getSize().x), cell_size / static_cast<float>(textures[board[x][y]].getSize().y) * -1);
			sprite.setPosition({cell_size * (x - 2) + (cell_size / 2.f), cell_size * (7 - (y - 2)) + (cell_size / 2)});
			tex.draw(sprite);
		}
	}
	sf::Sprite tex_spr(tex.getTexture());
	tex_spr.setPosition((window.getSize().x - tex.getSize().x) / 2.f, 0);
	window.draw(tex_spr);
}

void Board::generate_moves(Color color)
{
	for (int8_t x = 2; x < 10; x++)
		for (int8_t y = 2; y < 10; y++)
			if (get_color(board[x][y]) == color)
				generate_piece_moves(x, y, color);
}

const std::list<Position>& Board::get_offsets(Piece_type type)
{
	static std::list<Position> white_pawn_off = { {0, -1}, {0, -2}, {1, -1}, {-1, -1} };
	static std::list<Position> black_pawn_off = { {0, 1}, {0, 2}, {1, 1}, {-1, 1} };
	static std::list<Position> knight_off = { {2, 1}, {2, -1}, {-2, -1}, {-2, 1}, {1, 2}, {-1, 2}, {-1, -2}, {1, -2} };
	static std::list<Position> bishop_off = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
	static std::list<Position> rook_off = { {0, 1}, {0, -1}, {-1, 0}, {1, 0} };
	static std::list<Position> queen_off = { {0, 1}, {0, -1}, {-1, 0}, {1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
	static std::list<Position> king_off = { {0, 1}, {0, -1}, {-1, 0}, {1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

	int8_t abs = std::abs(type);
	switch (abs)
	{
		case Piece_type::White_pawn:
			return type == Piece_type::Black_pawn ? black_pawn_off : white_pawn_off;
		case Piece_type::White_knight:
			return knight_off;
		case Piece_type::White_bishop:
			return bishop_off;
		case Piece_type::White_rook:
			return rook_off;
		case Piece_type::White_queen:
			return queen_off;
		case Piece_type::White_king:
			return king_off;
		default:
			return white_pawn_off;
	}
	return white_pawn_off;
}

void Board::generate_pawn_moves(int8_t x, int8_t y, Color color)
{
	std::list<Move>& moves = (color == Color::White ? white_moves : black_moves);
	Color enemy_color = (color == Color::White ? Color::Black : Color::White);

	auto offsets = get_offsets(board[x][y]);
	auto offset = offsets.begin();
	Piece_type at_new_pos = board[x + offset->x][y + offset->y];

	if (at_new_pos == Piece_type::No_piece)
	{
		moves.push_back({ {x, y}, {static_cast<int8_t>(x + offset->x), static_cast<int8_t>(y + offset->y)}});
		offset++;
		at_new_pos = board[x + offset->x][y + offset->y];
		if (at_new_pos == Piece_type::No_piece && ((color == Color::White && y == 8) || (color == Black && y == 3)))
			moves.push_back({ {x, y}, {static_cast<int8_t>(x + offset->x), static_cast<int8_t>(y + offset->y)}});
		offset++;
	}
	else
	{
		offset++;
		offset++;
	}
	at_new_pos = board[x + offset->x][y + offset->y];

	if (Position(x + offset->x, y + offset->y) == en_passant || (at_new_pos != Piece_type::Out_of_range && at_new_pos != Piece_type::No_piece && get_color(at_new_pos) == enemy_color))
		moves.push_back({ {x, y}, {static_cast<int8_t>(x + offset->x), static_cast<int8_t>(y + offset->y)}});
	offset++;
	at_new_pos = board[x + offset->x][y + offset->y];
	if (Position(x + offset->x, y + offset->y) == en_passant || (at_new_pos != Piece_type::Out_of_range && at_new_pos != Piece_type::No_piece && get_color(at_new_pos) == enemy_color))
		moves.push_back({ {x, y}, {static_cast<int8_t>(x + offset->x), static_cast<int8_t>(y + offset->y)}});
}

void Board::generate_piece_moves(int8_t x, int8_t y, Color color)
{
	std::list<Move>& moves = (color == Color::White ? white_moves : black_moves);
	Color enemy_color = (color == Color::White ? Color::Black : Color::White);
	int8_t abs = std::abs(board[x][y]);

	if (abs == Piece_type::White_pawn)
		return generate_pawn_moves(x, y, color);

	bool linear = abs == Piece_type::White_rook || abs == Piece_type::White_bishop || abs == Piece_type::White_queen;

	for (auto& offset : get_offsets(board[x][y]))
	{
		Position actual = {x, y};
		do
		{
			Piece_type at_new_pos = board[actual.x + offset.x][actual.y + offset.y];
			if (at_new_pos == Piece_type::Out_of_range || get_color(at_new_pos) == color)
				break;
			moves.push_back({{x, y}, actual + offset});
			if (get_color(at_new_pos) == enemy_color)
				break;
			actual = actual + offset;
		} while (linear);
	}

	if (abs == Piece_type::White_king)
	{
		// Grand roque
		if (allowed_castle[color][0] && board[3][y] == Piece_type::No_piece && board[4][y] == Piece_type::No_piece && board[5][y] == Piece_type::No_piece)
			moves.push_back({ {x, y}, Position(x - 2, y) });

		// Petit roque
		if (allowed_castle[color][1] && board[7][y] == Piece_type::No_piece && board[8][y] == Piece_type::No_piece)
			moves.push_back({ {x, y}, Position(x + 2, y) });
	}
}

void Board::draw_moves(sf::RenderWindow& window, std::map<Piece_type, sf::Texture>& textures, float cell_size)
{
	sf::RectangleShape cell;
	sf::RenderTexture tex;
	std::list<Move>& moves = (player_turn == Color::White ? white_moves : black_moves);

	tex.create(cell_size * 8, cell_size * 8);

	cell.setSize({cell_size, cell_size});
	cell.setFillColor(sf::Color(255, 0, 0, 100));

	for (auto& move : moves)
	{
		cell.setPosition({(move.target.x - 2) * cell_size, (9 - move.target.y) * cell_size });
		tex.draw(cell);
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
		auto& moves = (player_turn == Color::White ? white_moves : black_moves);
		for (auto& move : moves)
		{
			if (move.target == Position(cell_pos.x + 2, cell_pos.y + 2) && move.start == Position(clicked_cell.x + 2, clicked_cell.y + 2))
			{
				move_piece(move);
				player_turn = Color(player_turn * -1);
				generate_moves(player_turn);
				break;
			}
		}
		clicked_cell = Position::invalid;
		return;
	}

	if (get_color(board[cell_pos.x + 2][cell_pos.y + 2]) == player_turn)
		clicked_cell = cell_pos;
}
