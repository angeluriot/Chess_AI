#include "Board.h"

void Board::init_board(const std::string& fen)
{
	// Ajoute les out_of_range sur les bordures et no_Piece partout ailleurs
	for (uint8_t x = 0; x < 2; x++)
		std::fill(board[x].begin(), board[x].end(), Type::Out_Of_Range);
	for (uint8_t x = 10; x < 12; x++)
		std::fill(board[x].begin(), board[x].end(), Type::Out_Of_Range);
	for (uint8_t x = 2; x < 10; x++)
		std::fill(board[x].begin() + 2, board[x].end() - 2, Type::No_Piece);

	// Lecture du fen pour placer les pièces
	uint8_t x = 2, y = 2;
	size_t i = 0;

	for (; fen[i] && fen[i] != ' '; i++)
	{
		if (fen[i] == '/')
		{
			y++;
			x = 2;
		}
		else if (std::isdigit(fen[i]))
			x += (fen[i] - '0');
		else
		{
			board[x][y] = get_type_from_fen(fen[i]);
			x++;
		}
	}
	i++;

	player_turn = (fen[i] == 'w' ? Color::White : Color::Black);
}

Board::Board(const std::string& fen)
{
	init_board(fen);

	en_passant = Position::invalid;

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

Type& Board::operator[](const Position& position)
{
	return board[position.x][position.y];
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
	return (get_color(at(move.target)) == color ? -get_value(at(move.target)) : get_value(at(move.target)));
}

void Board::clear_moves()
{
	white_moves.clear();
	black_moves.clear();
}

void Board::remove_illegal_moves(Color color)
{
}

void Board::handle_castling(const Move& move)
{
	Type piece = at(move.start);
	int8_t abs = std::abs(piece);
	Color enemy_color = get_color(piece) == Color::White ? Color::Black : Color::White;

	if (abs == Type::White_King)
	{
		// Grand roque
		if (move.target.x - move.start.x == -2)
		{
			board[5][move.start.y] = board[2][move.start.y];
			board[2][move.start.y] = Type::No_Piece;
		}

		// Petit roque
		if (move.target.x - move.start.x == 2)
		{
			board[7][move.start.y] = board[9][move.start.y];
			board[9][move.start.y] = Type::No_Piece;
		}
	}

	// Si le roi bouge il y a plus de roques
	if (abs == Type::White_King)
		allowed_castle[get_color(piece)] = { false, false };

	// Si une tour bouge on désactive un roque
	if (abs == Type::White_Rook && (move.start.x == 2 || move.start.x == 9))
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
	if (move.target == en_passant && std::abs(at(move.start)) == Type::White_Pawn)
		board[en_passant.x][en_passant.y + get_color(at(move.start))] = Type::No_Piece;

	if (std::abs(at(move.start)) == Type::White_Pawn && abs(move.target.y - move.start.y) > 1)
		en_passant = Position(move.start.x, move.start.y + (move.target.y - move.start.y) / 2);
	else
		en_passant = Position::invalid;

	// Déplacement de la pièce
	at(move.target) = board[move.start.x][move.start.y];
	at(move.start) = Type::No_Piece;

	// Promotion
	if (std::abs(at(move.target)) == Type::White_Pawn && (move.target.y == 9 || move.target.y == 2))
		at(move.target) = Type(Type::White_Queen * get_color(at(move.target)));

	generate_pins();

	clear_moves();
}

void Board::generate_pins()
{
	std::list<Position> linear_pieces;
	Position enemy_king;

	pins.clear();

	for (int8_t x = 2; x < 10; x++)
		for (int8_t y = 2; y < 10; y++)
		{
			if (std::abs(board[x][y]) == Type::White_King && get_color(board[x][y]) == Color(player_turn * -1))
				enemy_king = Position(x, y);
			if (is_linear(board[x][y]) && get_color(board[x][y]) == Color(player_turn))
				linear_pieces.push_back(Position(x, y));
		}

	for (auto& pos : linear_pieces)
	{
		std::list<Move> tmp_moves(1, Move(Position::invalid, pos));
		int8_t piece_cnt = 0;
		if (x_ray_attack(pos, enemy_king))
		{
			Position dir = Position(sign(enemy_king.x - pos.x), sign(enemy_king.y - pos.y));
			for (Position actual = pos + dir; at(actual) != Type::Out_Of_Range && piece_cnt < 2; actual = actual + dir)
			{
				if (actual == enemy_king)
					break;
				tmp_moves.push_back({pos, actual});
				if (get_color(at(actual)) == Color(player_turn * -1))
					piece_cnt++;
				else if (get_color(at(actual)) == player_turn)
				{
					piece_cnt = 2;
					break;
				}
			}
		}
		if (piece_cnt == 1)
			pins.insert({pos, tmp_moves});
	}
}

Type& Board::at(const Position& position)
{
	return board[position.x][position.y];
}

Board Board::get_moved_board(const Move& move)
{
	Board result(*this);
	result.move_piece(move);
	return result;
}

void Board::generate_moves(Color color)
{
	for (int8_t x = 2; x < 10; x++)
		for (int8_t y = 2; y < 10; y++)
			if (get_color(board[x][y]) == color)
				generate_piece_moves(x, y, color);
}

void Board::generate_pawn_moves(int8_t x, int8_t y, Color color)
{
	std::list<Move>& moves = (color == Color::White ? white_moves : black_moves);
	Color enemy_color = (color == Color::White ? Color::Black : Color::White);
	Position pos = {x, y};

	auto offsets = get_offsets(at(pos));
	auto offset = offsets.begin();

	if (at(pos + *offset) == Type::No_Piece)
	{
		if (is_legal({ pos, pos + *offset }))
			moves.push_back({ pos, pos + *offset });
		offset++;
		if (at(pos + *offset) == Type::No_Piece && ((color == Color::White && y == 8) || (color == Color::Black && y == 3)))
			if (is_legal({ pos, pos + *offset }))
				moves.push_back({ pos, pos + *offset });
		offset++;
	}
	else
	{
		offset++;
		offset++;
	}

	// Offsets de prise en diagonale
	if (Position(x + offset->x, y + offset->y) == en_passant || (at(pos + *offset) != Type::Out_Of_Range && at(pos + *offset) != Type::No_Piece && get_color(at(pos + *offset)) == enemy_color))
		if (is_legal({ pos, pos + *offset }))
			moves.push_back({ {x, y}, Position(x, y) + *offset });
	offset++;
	if (Position(x + offset->x, y + offset->y) == en_passant || (at(pos + *offset) != Type::Out_Of_Range && at(pos + *offset) != Type::No_Piece && get_color(at(pos + *offset)) == enemy_color))
		if (is_legal({ pos, pos + *offset }))
			moves.push_back({ {x, y}, Position(x, y) + *offset });
}

void Board::generate_piece_moves(int8_t x, int8_t y, Color color)
{
	int8_t abs = std::abs(board[x][y]);
	if (abs == Type::White_Pawn)
		return generate_pawn_moves(x, y, color);

	std::list<Move>& moves = get_player_moves();
	Color enemy_color = Color(player_turn * -1);
	bool linear = is_linear(board[x][y]);

	for (auto& offset : get_offsets(board[x][y]))
	{
		Position actual = {x, y};
		Position pinned = Position::invalid;
		std::list<Move> tmp_moves;
		do
		{
			if (at(actual + offset) == Type::Out_Of_Range || get_color(at(actual + offset)) == color)
				break;
			if (is_legal(Move({x, y}, actual + offset)))
				moves.push_back({{x, y}, actual + offset});
			if (get_color(at(actual + offset)) == enemy_color)
				break;
			actual = actual + offset;
		} while (linear);
	}

	if (abs == Type::White_King)
	{
		// Grand roque
		if (allowed_castle[color][0] && board[3][y] == Type::No_Piece && board[4][y] == Type::No_Piece && board[5][y] == Type::No_Piece)
			moves.push_back({ {x, y}, Position(x - 2, y) });

		// Petit roque
		if (allowed_castle[color][1] && board[7][y] == Type::No_Piece && board[8][y] == Type::No_Piece)
			moves.push_back({ {x, y}, Position(x + 2, y) });
	}
}

std::list<Move>& Board::get_player_moves()
{
	return (player_turn == Color::White ? white_moves : black_moves);
}

std::list<Move>& Board::get_enemy_moves()
{
	return (player_turn == Color::White ? white_moves : black_moves);
}

bool Board::x_ray_attack(Position pos, Position pos2)
{
	Type type = Type(std::abs(at(pos)));
	if (!is_linear(type))
		return false;
	if ((type == Type::White_Rook || type == Type::White_Queen) && (pos2.x == pos.x || pos2.y == pos.y))
		return true;
	if ((type == Type::White_Bishop || type == Type::White_Queen) && (std::abs(pos2.x - pos.x) == std::abs(pos2.y - pos.y)))
		return true;
	return false;
}

bool Board::is_legal(Move move)
{
	for (auto& pin : pins)
	{
		if (std::find_if(pin.second.begin(), pin.second.end(), [move](Move pin_move) -> bool { return move.start == pin_move.target; }) == pin.second.end())
			continue;
		if (std::find_if(pin.second.begin(), pin.second.end(), [move](Move pin_move) -> bool { return move.target == pin_move.target; }) == pin.second.end())
			return false;
	}
	return true;
}
