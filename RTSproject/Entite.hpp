#pragma once

#include <SFML/Graphics.hpp>

class CEntite {
protected :
	sf::Vector2f position;
	unsigned int age;
	unsigned int tempsDeVie;
	unsigned int tempsAvantReproduction;

public:
	CEntite(unsigned int age=0);
	sf::Vector2f getPosition() const { return position; }
	bool estVivant() const { return age < tempsDeVie; }
	virtual void afficher(sf::RenderWindow& window);
	virtual void seDeplacer();
	unsigned int getAge() const { return age; }
	unsigned int getTempsAvantReproduction() const { return tempsAvantReproduction; }
	void setTempsAvantReproduction(unsigned int temps) { tempsAvantReproduction = temps; }
};