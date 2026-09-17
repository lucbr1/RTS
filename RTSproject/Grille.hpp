#pragma once

#include <SFML/Graphics.hpp>
#include "Variable.hpp"
#include "Entite.hpp"

class CGrille {
	std::vector < std::vector < std::vector<CEntite* >>> grille;
	sf::VertexArray vertexGrille;
	sf::RenderWindow& window;
	Etape etape;
	void Reproduction();

public:
	CGrille(sf::RenderWindow& window);
	void afficher() const;
	void gameLoopProie();
	void gameLoopPredateur();
	unsigned int getNombreProies() const;
	void reset();
	void setEtape(Etape etape) { this->etape = etape; reset(); }
};