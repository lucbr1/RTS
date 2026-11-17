#pragma once

#include <SFML/Graphics.hpp>
#include "Variable.hpp"
#include "Entite.hpp"
#include "Cellule.hpp"

struct Animation {
	TypeAnimation type;
	sf::Vector2f position; // position de la cellule
	float debut;           // temps de départ en secondes
};

class CGrille {
	std::vector < std::vector < CCellule>> grille;
	std::vector<Animation> animations;
	sf::Clock horloge;
	void ajouterAnimation(TypeAnimation type, sf::Vector2f position);
	sf::VertexArray vertexGrille;
	sf::RenderWindow& window;
	Etape etape;
	void Reproduction();
	unsigned int nbProies = 0;
	unsigned int nbPredateurs = 0;

public:
	CGrille(sf::RenderWindow& window);
	void afficher() const;
	void afficherAnimations(const sf::Font& font);
	void gameLoop();
	void gameLoopProie();
	void gameLoopPredateur();
	void gameLoopProiePredateur();


	void deplacementPredateurs(bool useSatiete);
	void deplacementProies(bool useSatiete);
	void reset();
	void setEtape(Etape etape) { this->etape = etape; reset(); }
	unsigned int getNbProies() const { return nbProies; }
	unsigned int getNbPredateurs() const { return nbPredateurs; }
};