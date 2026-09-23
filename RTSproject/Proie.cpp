#include "Proie.hpp"

CProie::CProie(int x, int y, unsigned int age) : CEntite(age) {
	unsigned int numLinesX = (windowWidth / cellSize)-1;
	unsigned int numLinesY = (windowHeight / cellSize)-1;

	position = sf::Vector2f(static_cast<float>(cellSize*x), static_cast<float>(cellSize*y));
	age = 0;
	mort = false;
}


void CProie::afficher(sf::RenderWindow& window) {
	static sf::CircleShape proie(cellSize/2.0f);
	proie.setFillColor(sf::Color::Green);
	proie.setPosition(position);
	window.draw(proie);
}
