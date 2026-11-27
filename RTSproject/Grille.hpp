#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Variable.hpp"
#include "Cellule.hpp"

struct Animation {
	TypeAnimation type;
	sf::Vector2f position; // position de la cellule
	float debut;           // en secondes
};

class CGrille {
	sf::RenderWindow& window;
	std::vector<std::vector<CCellule>> grille; // grille[y][x]
	sf::VertexArray vertexGrille;
	std::vector<Animation> animations;
	sf::Clock horloge;
	unsigned int nbProies = 0;
	unsigned int nbPredateurs = 0;

	void ajouterAnimation(TypeAnimation type, sf::Vector2f position);
	void vider();
	void construireGrille();
	void deplacement();
	void retirerMorts(CCellule& cellule);
	void combatPredateurs(CCellule& cellule);
	void combatEtReproductionPredateurs(CCellule& cellule, int x, int y);
	void naissancePredateur(CCellule& cellule, CPredateur* male, CPredateur* femelle, int x, int y);
	void predation(CCellule& cellule);
	void reproductionProies();
	void compterPopulations();
	void mettreAJourHerbe();

	void gameLoopProie();
	void gameLoopPredateur();
	void gameLoopProiePredateur();

public:
	CGrille(sf::RenderWindow& window);
	~CGrille();
	CGrille(const CGrille&) = delete; // la grille possède ses entités (new / delete)
	CGrille& operator=(const CGrille&) = delete;

	void reset();
	void gameLoop();
	void afficher() const;
	void afficherAnimations(const sf::Font& font);
	unsigned int getNbProies() const { return nbProies; }
	unsigned int getNbPredateurs() const { return nbPredateurs; }
};
