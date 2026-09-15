#pragma once
#include <SFML/Graphics.hpp>
#include "Proie.hpp"
#include <random>

int randomizer(int min, int max);

void gameLoopProie(sf::RenderWindow& window, std::vector<CProie*>& proies);