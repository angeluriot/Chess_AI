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
 
class Piece;

extern uint16_t	screen_width;						// Largeur de l'écran

#define		WIDTH	1920							// Largeur de l'écran
#define		HEIGHT	1080							// Hauteur de l'écran
#define		RESIZE	((1. / 1920.) * screen_width)	// Coéfficient en fonction de la taille de l'écran

#define		PIECE_NULL static_cast<Piece*>(NULL)

void		event_check(sf::Event& sf_event, sf::RenderWindow& window, bool& end);
float		random_float(float min, float max);
uint32_t	random_int(int32_t min, int32_t max);
bool		rand_probability(const double& probability);

#endif
