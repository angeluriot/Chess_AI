#ifndef UTILS_H
#define UTILS_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <random>
#include <vector>
#include <iostream>
#include <list>
#include <array>
#include <algorithm>
#include <functional>
#include <numeric>
#include <string>
#include <unordered_map>

extern uint16_t	screen_width;						// Largeur de l'écran

#define		WIDTH	1920							// Largeur de l'écran
#define		HEIGHT	1080							// Hauteur de l'écran
#define		RESIZE	((1. / 1920.) * screen_width)	// Coéfficient en fonction de la taille de l'écran

void		event_check(sf::Event& sf_event, sf::RenderWindow& window, bool& end);
float		random_float(float min, float max);
uint32_t	random_int(int32_t min, int32_t max);
bool		rand_probability(const double& probability);
uint32_t	random_32();
uint64_t	random_64();

template<class T>
T sign(const T& a)
{
	return (T(0) < a) - (a < T(0));
}

#endif
