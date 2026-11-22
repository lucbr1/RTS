#include "Predateur.hpp"
#include "fonctions.hpp"
#include <iostream>

CPredateur::CPredateur(int x, int y, unsigned int age) : CEntite(age) {
	unsigned int numLinesX = (windowWidth / cellSize) - 1;
	unsigned int numLinesY = (windowHeight / cellSize) - 1;

	position = sf::Vector2f(static_cast<float>(cellSize * x), static_cast<float>(cellSize * y));
	mort = false;
}

void CPredateur::afficher(sf::RenderWindow& window) {
	static bool texturePredateurChargee = false;
	static sf::Texture texturePredateur;
	if (!texturePredateurChargee) {
		if (!texturePredateur.loadFromFile("assets/loup.png")) {
			// Gestion de l'erreur si le chargement échoue
			std::cerr << "Erreur lors du chargement de l'image loup.png" << std::endl;
		}
		texturePredateurChargee = true;
	}

	static sf::Sprite predateur(texturePredateur);

	// 1. Récupérer la taille réelle de l'image source (en pixels)
	sf::FloatRect bounds = predateur.getLocalBounds();

	// 2. Calculer l'échelle pour que le sprite fasse exactement "cellSize" de large et de haut
	float scaleX = cellSize / bounds.size.x;
	float scaleY = cellSize / bounds.size.y;
	predateur.setScale(sf::Vector2f(scaleX, scaleY));

	// 3. Appliquer la position
	predateur.setPosition(position);
	window.draw(predateur);
}

void CPredateur::seDeplacer(bool useSatiete) {
	if (useSatiete && satiete <= 0)
	{
		mort = true;
		return;
	}
	else if (useSatiete)
	{
		satiete--;
	}
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
	age++;
}
