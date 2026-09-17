#include "Entite.hpp"
#include "fonctions.hpp"
#include "Variable.hpp"

CEntite::CEntite(unsigned int annee) {
	unsigned int numLinesX = (windowWidth / cellSize) - 1;
	unsigned int numLinesY = (windowHeight / cellSize) - 1;

	position = sf::Vector2f(static_cast<float>(cellSize * randomizer(0, numLinesX)), static_cast<float>(cellSize * randomizer(0, numLinesY)));
	age = annee;
	tempsDeVie = 100;
	tempsAvantReproduction = 0;
}

void CEntite::afficher(sf::RenderWindow& window) {
	sf::CircleShape proie(cellSize / 2.0f);
	proie.setFillColor(sf::Color::Green);
	proie.setPosition(position);
	window.draw(proie);
}

void CEntite::seDeplacer()
{
	int maxX = (windowWidth / cellSize) - 1;
	int maxY = (windowHeight / cellSize) - 1;


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

	if (tempsAvantReproduction > 0)
		tempsAvantReproduction--;
	age ++;
}