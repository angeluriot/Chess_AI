#include "Board.h"

Board::Board()
{
	pieces.clear();

	for (auto& square : board)
		std::fill(square.begin(), square.end(), PIECE_NULL);

	pieces = {
		Piece(&PieceType::rook, Black, { 0, 0 }, this),
		Piece(&PieceType::knight, Black, { 1, 0 }, this),
		Piece(&PieceType::bishop, Black, { 2, 0 }, this),
		Piece(&PieceType::queen, Black, { 3, 0 }, this),
		Piece(&PieceType::king, Black, { 4, 0 }, this),
		Piece(&PieceType::bishop, Black, { 5, 0 }, this),
		Piece(&PieceType::knight, Black, { 6, 0 }, this),
		Piece(&PieceType::rook, Black, { 7, 0 }, this),

		Piece(&PieceType::black_pawn, Black, { 0, 1 }, this),
		Piece(&PieceType::black_pawn, Black, { 1, 1 }, this),
		Piece(&PieceType::black_pawn, Black, { 2, 1 }, this),
		Piece(&PieceType::black_pawn, Black, { 3, 1 }, this),
		Piece(&PieceType::black_pawn, Black, { 4, 1 }, this),
		Piece(&PieceType::black_pawn, Black, { 5, 1 }, this),
		Piece(&PieceType::black_pawn, Black, { 6, 1 }, this),
		Piece(&PieceType::black_pawn, Black, { 7, 1 }, this),

		Piece(&PieceType::white_pawn, White, { 0, 6 }, this),
		Piece(&PieceType::white_pawn, White, { 1, 6 }, this),
		Piece(&PieceType::white_pawn, White, { 2, 6 }, this),
		Piece(&PieceType::white_pawn, White, { 3, 6 }, this),
		Piece(&PieceType::white_pawn, White, { 4, 6 }, this),
		Piece(&PieceType::white_pawn, White, { 5, 6 }, this),
		Piece(&PieceType::white_pawn, White, { 6, 6 }, this),
		Piece(&PieceType::white_pawn, White, { 7, 6 }, this),

		Piece(&PieceType::rook, White, { 0, 7 }, this),
		Piece(&PieceType::knight, White, { 1, 7 }, this),
		Piece(&PieceType::bishop, White, { 2, 7 }, this),
		Piece(&PieceType::queen, White, { 3, 7 }, this),
		Piece(&PieceType::king, White, { 4, 7 }, this),
		Piece(&PieceType::bishop, White, { 5, 7 }, this),
		Piece(&PieceType::knight, White, { 6, 7 }, this),
		Piece(&PieceType::rook, White, { 7, 7 }, this)
	};

	for (auto& piece : pieces)
		(*this)[piece.pos] = &piece;

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

	std::map<PieceType::Type, sf::Texture> black_sprites = {
		{ PieceType::Type::Pawn, black_pawn }, { PieceType::Type::Rook, black_rook }, { PieceType::Type::Knight, black_knight },
		{ PieceType::Type::Bishop, black_bishop }, { PieceType::Type::Queen, black_queen }, { PieceType::Type::King, black_king }
	};

	std::map<PieceType::Type, sf::Texture> white_sprites = {
		{ PieceType::Type::Pawn, white_pawn }, { PieceType::Type::Rook, white_rook }, { PieceType::Type::Knight, white_knight },
		{ PieceType::Type::Bishop, white_bishop }, { PieceType::Type::Queen, white_queen }, { PieceType::Type::King, white_king }
	};

	sprites = {
		{ PieceColor::Black, black_sprites }, { PieceColor::White, white_sprites },
	};

	en_passant = Position::invalid;
	player_turn = PieceColor::White;

	update_moves(player_turn);
}

Board::Board(const Board& other)
{
	*this = other;
}

void Board::operator=(const Board& other)
{
	pieces = other.pieces;
	for (auto& piece : pieces)
		piece.board = this;

	for (auto& square : board)
		std::fill(square.begin(), square.end(), PIECE_NULL);

	for (auto& piece : pieces)
		(*this)[piece.pos] = &piece;

	en_passant = other.en_passant;
	player_turn = other.player_turn;
	clicked_cell = Position::invalid;
	sprites = other.sprites;
}

Piece*& Board::operator[](Position position)
{
	return board[position.x][position.y];
}

uint16_t Board::get_score(PieceColor color)
{
	return std::accumulate(pieces.begin(), pieces.end(), uint16_t(), [color](uint16_t sum, Piece& piece) -> uint16_t { return sum + (piece.color == color ? piece.type->value : 0); });
}

int16_t Board::move_score(const Move& move, PieceColor color)
{
	if ((*this)[move.target])
		return ((*this)[move.target]->color == color ? -(*this)[move.target]->type->value : (*this)[move.target]->type->value);

	return 0;
}

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

void Board::update_moves(PieceColor color)
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

void Board::move_piece(Move move)
{
	(*this)[move.start]->setPos(move.target);
	player_turn = (player_turn == PieceColor::Black ? PieceColor::White : PieceColor::Black);
	for (auto& piece : pieces)
		piece.moves.clear();
	update_moves(player_turn);
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
