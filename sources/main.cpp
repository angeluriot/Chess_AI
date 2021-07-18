#include "utils.h"
#include "BitBoard.h"
#include "Computer.h"

// Vérifie les évenements

void event_check(sf::Event& sf_event, sf::RenderWindow& window, bool& end)
{
	while (window.pollEvent(sf_event))
		if (sf_event.type == sf::Event::Closed)
		{
			window.close();
			end = true;
			return;
		}
}

// Initialisation de la fenêtre en fonction de l'écran

void init_window(sf::RenderWindow& window, std::string project_name)
{
	uint16_t width;
	uint16_t height;

	if (sf::VideoMode::getDesktopMode().width > (16. / 9.) * sf::VideoMode::getDesktopMode().height)
		height = (sf::VideoMode::getDesktopMode().height * 3) / 4, width = (height * 16) / 9;

	else if (sf::VideoMode::getDesktopMode().width < (16. / 9.) * sf::VideoMode::getDesktopMode().height)
		width = (sf::VideoMode::getDesktopMode().width * 3) / 4, height = (width * 9) / 16;

	else
		width = (sf::VideoMode::getDesktopMode().width * 3) / 4, height = (sf::VideoMode::getDesktopMode().height * 3) / 4;

	screen_width = width;

	sf::ContextSettings settings;
	settings.antialiasingLevel = 0;
	settings.depthBits = 16;
	settings.majorVersion = 3;

	window.create(sf::VideoMode(width, height), project_name, sf::Style::Close | sf::Style::Titlebar, settings);

	sf::Image icon;
	icon.loadFromFile("dependencies/resources/icon.png");

	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
}

void draw_grid(sf::RenderTexture& tex, float cell_size)
{
	sf::RectangleShape cell;

	cell.setSize({cell_size, cell_size});
	cell.setFillColor(sf::Color(90, 137, 181));
	for (int8_t i = 0; i < 64; i++)
	{
		cell.setPosition({(i % 8) * cell_size, (i / 8) * cell_size });
		tex.draw(cell);
		if (i % 8 != 7)
			cell.setFillColor(cell.getFillColor() == sf::Color(234, 234, 210) ? sf::Color(90, 137, 181) : sf::Color(234, 234, 210));
	}
}

// Main

int main()
{
	srand(time(NULL));
	sf::RenderWindow window;
	init_window(window, "Chess AI");
	bool end = false;
	sf::RenderTexture grid_tex;
	float cell_size = std::min(window.getSize().x, window.getSize().y) / 8.f;

	bool space_pressed = false;
	bool left_pressed = false, up_pressed = false, down_pressed = false;

	grid_tex.create(cell_size * 8, cell_size * 8);
	draw_grid(grid_tex, cell_size);

	sf::Sprite grid_spr(grid_tex.getTexture());
	grid_spr.setPosition({(window.getSize().x - grid_tex.getSize().x) / 2.f, (window.getSize().y - grid_tex.getSize().y) / 2.f});

	BitBoard board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	BitBoard last_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	Computer computer;

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

	std::map<Piece, sf::Texture> textures = {
		{ Piece::Black_Pawn, black_pawn }, { Piece::Black_Rook, black_rook }, { Piece::Black_Knight, black_knight }, { Piece::Black_Bishop, black_bishop },
		{ Piece::Black_Queen, black_queen }, { Piece::Black_King, black_king }, { Piece::White_Pawn, white_pawn }, { Piece::White_Rook, white_rook },
		{ Piece::White_Knight, white_knight }, { Piece::White_Bishop, white_bishop }, { Piece::White_Queen, white_queen }, { Piece::White_King, white_king }
	};

	sf::Event event;
	while(!end)
	{
		event_check(event, window, end);
		window.clear();
		window.draw(grid_spr);

		board.draw_last_move(window, textures, cell_size);
		board.draw_pieces(window, textures, cell_size);
		//board.draw_moves(window, textures, cell_size);
		//board.draw_pins(window, textures, cell_size);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			if (!space_pressed)
			{
				last_board = board;
				computer.move(board);
			}
			space_pressed = true;
		}
		else
			space_pressed = false;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			if (!left_pressed)
				board = last_board;
			left_pressed = true;
		}
		else
			left_pressed = false;

		board.check_click_on_piece(window, cell_size, &last_board);

		window.display();
	}
}
