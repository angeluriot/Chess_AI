#include "Board.h"

void Board::draw_pieces(sf::RenderWindow& window, std::map<Type, sf::Texture>& textures, float cell_size)
{
	sf::RenderTexture tex;

	tex.create(cell_size * 8, cell_size * 8);
	for (uint8_t x = 2; x < board.size() - 2; x++)
		for (uint8_t y = 2; y < board[x].size() - 2; y++)
		{
			sf::Sprite sprite = sf::Sprite(textures[board[x][y]]);
			sprite.setOrigin({ textures[board[x][y]].getSize().x / 2.f, textures[board[x][y]].getSize().y / 2.f});
			sprite.setScale(cell_size / static_cast<float>(textures[board[x][y]].getSize().x), cell_size / static_cast<float>(textures[board[x][y]].getSize().y) * -1);
			sprite.setPosition({cell_size * (x - 2) + (cell_size / 2.f), cell_size * (7 - (y - 2)) + (cell_size / 2)});
			tex.draw(sprite);
		}

	sf::Sprite tex_spr(tex.getTexture());
	tex_spr.setPosition((window.getSize().x - tex.getSize().x) / 2.f, 0);
	window.draw(tex_spr);
}

void Board::draw_moves(sf::RenderWindow& window, std::map<Type, sf::Texture>& textures, float cell_size)
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
				remove_illegal_moves(player_turn);
				break;
			}
		}
		clicked_cell = Position::invalid;
		return;
	}

	if (get_color(board[cell_pos.x + 2][cell_pos.y + 2]) == player_turn)
		clicked_cell = cell_pos;
}

void Board::draw_pins(sf::RenderWindow& window, std::map<Type, sf::Texture>& textures, float cell_size)
{
	sf::RenderTexture tex;
	sf::RectangleShape cell;

	tex.create(cell_size * 8, cell_size * 8);

	cell.setSize({cell_size, cell_size});
	cell.setFillColor(sf::Color(255, 255, 0, 100));

	for (auto& pin : pins)
	{
		for (auto& move : pin.second)
		{
			cell.setPosition({(move.target.x - 2) * cell_size, (9 - move.target.y) * cell_size });
			tex.draw(cell);
		}
	}
	sf::Sprite sprite(tex.getTexture());
	sprite.setPosition({(window.getSize().x - tex.getSize().x) / 2.f, (window.getSize().y - tex.getSize().y) / 2.f});
	window.draw(sprite);
}
