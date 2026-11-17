#include "Proie.hpp"
#include <iostream>

CProie::CProie(int x, int y, unsigned int age) : CEntite(age) {
	unsigned int numLinesX = (windowWidth / cellSize)-1;
	unsigned int numLinesY = (windowHeight / cellSize)-1;

	position = sf::Vector2f(static_cast<float>(cellSize*x), static_cast<float>(cellSize*y));
	age = 0;
	mort = false;
}


void CProie::afficher(sf::RenderWindow& window) {
	static bool textureProieChargee = false;
	static sf::Texture textureProie;
	if (!textureProieChargee) {
		if (!textureProie.loadFromFile("assets/chevreuil.png")) {
			// Gestion de l'erreur si le chargement échoue
			std::cerr << "Erreur lors du chargement de l'image chevreuil.png" << std::endl;
		}
		textureProieChargee = true;
	}

	static sf::Sprite proie(textureProie);

	// 1. Récupérer la taille réelle de l'image source (en pixels)
	sf::FloatRect bounds = proie.getLocalBounds();

	// 2. Calculer l'échelle pour que le sprite fasse exactement "cellSize" de large et de haut
	float scaleX = cellSize / bounds.size.x;
	float scaleY = cellSize / bounds.size.y;
	proie.setScale(sf::Vector2f(scaleX, scaleY));

	// 4. Appliquer la position
	proie.setPosition(position);
	window.draw(proie);
}