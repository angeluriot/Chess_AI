#include "Board.h"

void Board::init_board(const std::string& fen)
{
	// Ajoute les out_of_range sur les bordures et no_Piece partout ailleurs
	for (uint8_t x = 0; x < 12; x++)
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
	en_passant = other.en_passant;
	player_turn = other.player_turn;
	allowed_castle = other.allowed_castle;
	last_move = other.last_move;
}

void Board::operator=(const Board& other)
{
	board = other.board;
	en_passant = other.en_passant;
	player_turn = other.player_turn;
	allowed_castle = other.allowed_castle;
	last_move = other.last_move;
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
	if (std::abs(at(move.target)) == Type::White_Rook && (move.target == Position(2, 2) || move.target == Position(2, 9) || move.target == Position(9, 2) || move.target == Position(9, 9)))
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

	last_move = move;
}
