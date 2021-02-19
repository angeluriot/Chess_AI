#include "utils.h"

uint16_t screen_width;

// V�rifie les �venements

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

// Donne un entier entre min et max

uint32_t random_int(int32_t min, int32_t max)
{
	return rand() % (max - min) + min;
}

// Donne "vrai" avec une probabilit� choisie

bool rand_probability(const double& probability)
{
	if (probability < 0.)
		return false;

	if (probability < 1.)
		return (rand() / static_cast<double>(RAND_MAX) < probability);

	return true;
}
