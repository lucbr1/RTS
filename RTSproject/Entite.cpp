#include "Entite.hpp"
#include "fonctions.hpp"
#include <iostream>

CEntite::CEntite(int x, int y, unsigned int age)
	: position(static_cast<float>(x * cellSize), static_cast<float>(y * cellSize)), age(age)
{
	sexe = (randomizer(0, 1) == 0) ? Sexe::Male : Sexe::Femelle;
}

sf::Texture chargerTexture(const std::string& chemin)
{
	sf::Texture texture;
	if (!texture.loadFromFile(chemin))
		std::cerr << "Erreur lors du chargement de " << chemin << std::endl;
	return texture;
}

void CEntite::afficher(sf::RenderWindow& window) const
{
	sf::Sprite sprite(getTexture());
	sf::Vector2f taille = sprite.getLocalBounds().size;
	sprite.setScale({ cellSize / taille.x, cellSize / taille.y });
	sprite.setPosition(position);
	window.draw(sprite);
}

// déplacement d'une case (diagonales comprises), la grille est un tore
void CEntite::seDeplacer()
{
	const int nbColonnes = windowWidth / cellSize;
	const int nbLignes = windowHeight / cellSize;

	int x = static_cast<int>(position.x / cellSize) + randomizer(-1, 1);
	int y = static_cast<int>(position.y / cellSize) + randomizer(-1, 1);

	x = (x + nbColonnes) % nbColonnes;
	y = (y + nbLignes) % nbLignes;

	position.x = static_cast<float>(x * cellSize);
	position.y = static_cast<float>(y * cellSize);

	if (tempsAvantReproduction > 0)
		tempsAvantReproduction--;
	age++;
}
