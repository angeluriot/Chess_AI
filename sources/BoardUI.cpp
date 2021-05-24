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
	auto moves = generate_moves(player_turn);

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

void Board::check_click_on_piece(const sf::RenderWindow& window, float cell_size, Board* last_board = nullptr)
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
		auto moves = generate_moves(player_turn);
		auto move_it = std::find(moves.begin(), moves.end(), Move(Position(clicked_cell.x + 2, clicked_cell.y + 2), Position(cell_pos.x + 2, cell_pos.y + 2)));
		if (move_it != moves.end())
		{
			*last_board = *this;
			move_piece(*move_it);
			player_turn = Color(player_turn * -1);
		}
		clicked_cell = Position::invalid;
		return;
	}

	if (get_color(board[cell_pos.x + 2][cell_pos.y + 2]) == player_turn)
		clicked_cell = cell_pos;
}

void Board::draw_pins(sf::RenderWindow& window, std::map<Type, sf::Texture>& textures, float cell_size)
{
	/*sf::RenderTexture tex;
	sf::RectangleShape cell;

	tex.create(cell_size * 8, cell_size * 8);

	cell.setSize({cell_size, cell_size});
	cell.setFillColor(sf::Color(255, 255, 0, 100));

	for (auto& pin : get_color_pins(Color(player_turn * -1)))
	{
		for (auto& move : pin.second)
		{
			cell.setPosition({(move.target.x - 2) * cell_size, (9 - move.target.y) * cell_size });
			tex.draw(cell);
		}
	}
	sf::Sprite sprite(tex.getTexture());
	sprite.setPosition({(window.getSize().x - tex.getSize().x) / 2.f, (window.getSize().y - tex.getSize().y) / 2.f});
	window.draw(sprite);*/
}

void Board::draw_last_move(sf::RenderWindow& window, std::map<Type, sf::Texture>& textures, float cell_size)
{
	sf::RenderTexture tex;
	sf::RectangleShape cell;

	tex.create(cell_size * 8, cell_size * 8);

	cell.setSize({cell_size, cell_size});
	cell.setFillColor(sf::Color(255, 255, 0, 150));

	cell.setPosition({(last_move.target.x - 2) * cell_size, (9 - last_move.target.y) * cell_size });
	tex.draw(cell);
	cell.setPosition({(last_move.start.x - 2) * cell_size, (9 - last_move.start.y) * cell_size });
	tex.draw(cell);

	sf::Sprite sprite(tex.getTexture());
	sprite.setPosition({(window.getSize().x - tex.getSize().x) / 2.f, (window.getSize().y - tex.getSize().y) / 2.f});
	window.draw(sprite);
}
