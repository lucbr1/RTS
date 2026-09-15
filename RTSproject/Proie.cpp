#include "Proie.hpp"
#include <iostream>
#include "fonctions.hpp"

CProie::CProie() {
	unsigned int numLinesX = (windowWidth / cellSize)-1;
	unsigned int numLinesY = (windowHeight / cellSize)-1;

	position = sf::Vector2f(static_cast<float>(cellSize*randomizer(0, numLinesX)), static_cast<float>(cellSize*randomizer(0, numLinesY)));
	age = 0;
	tempsDeVie = 100;
}

int CProie::seDeplacer()
{
	int nbReproductions = 0;
	int maxX = (windowWidth / cellSize) - 1;
	int maxY = (windowHeight / cellSize) - 1;

	for (size_t i = 0; i < t; i++)
	{
		int dx = randomizer(-1, 1);
		int dy = randomizer(-1, 1);

		int x = static_cast<int>(position.x / cellSize);
		int y = static_cast<int>(position.y / cellSize);

		x += dx;
		y += dy;

		x = (x % (maxX + 1) + (maxX + 1)) % (maxX + 1);
		y = (y % (maxY + 1) + (maxY + 1)) % (maxY + 1);

		position.x = static_cast<float>(x * cellSize);
		position.y = static_cast<float>(y * cellSize);

		nbReproductions += (randomizer(0, 100) < tauxReproductionH * 100) ? 1 : 0;
	}

	age += t;
	return nbReproductions;
}


void CProie::afficher(sf::RenderWindow& window) {
	sf::CircleShape proie(cellSize/2.0f);
	proie.setFillColor(sf::Color::Green);
	proie.setPosition(position);
	window.draw(proie);
}
