#pragma once

#include <SFML/Graphics.hpp>
#include "Variable.hpp"
#include "Entite.hpp"

class CGrille {
	std::vector < std::vector < std::vector<CEntite* >>> grille;
	sf::VertexArray vertexGrille;
	sf::RenderWindow& window;
	void Reproduction(Etape etape);

public:
	CGrille(sf::RenderWindow& window);
	void afficher() const;
	void gameLoopProie(Etape etape);
	unsigned int getNombreProies() const;
	void reset();
};