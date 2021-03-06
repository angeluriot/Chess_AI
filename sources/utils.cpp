#include "utils.h"

uint16_t screen_width;

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

// Donne un réel entre min et max

float random_float(float min, float max)
{
	return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (max - min) + min;
}

// Donne un entier entre min et max

uint32_t random_int(int32_t min, int32_t max)
{
	return rand() % (max - min) + min;
}

// Donne "vrai" avec une probabilité choisie

bool rand_probability(const double& probability)
{
	if (probability < 0.)
		return false;

	if (probability < 1.)
		return (rand() / static_cast<double>(RAND_MAX) < probability);

	return true;
}
