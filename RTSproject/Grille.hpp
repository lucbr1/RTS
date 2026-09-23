#pragma once

#include <SFML/Graphics.hpp>
#include "Variable.hpp"
#include "Entite.hpp"
#include "Cellule.hpp"

class CGrille {
	std::vector < std::vector < CCellule>> grille;
	sf::VertexArray vertexGrille;
	sf::RenderWindow& window;
	Etape etape;
	void Reproduction();
	unsigned int nbProies = 0;
	unsigned int nbPredateurs = 0;

public:
	CGrille(sf::RenderWindow& window);
	void afficher() const;
	void gameLoop();
	void gameLoopProie();
	void gameLoopPredateur();
	void reset();
	void setEtape(Etape etape) { this->etape = etape; reset(); }
	unsigned int getNbProies() const { return nbProies; }
	unsigned int getNbPredateurs() const { return nbPredateurs; }
};