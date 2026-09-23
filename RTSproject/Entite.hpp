#pragma once

#include <SFML/Graphics.hpp>
#include "Variable.hpp"


class CEntite {
protected :
	sf::Vector2f position;
	unsigned int age;
	bool mort = false;
	unsigned int tempsAvantReproduction;
	Sexe sexe;

public:
	CEntite(unsigned int age=0);
	virtual ~CEntite() = default;
	sf::Vector2f getPosition() const { return position; }
	bool estVivant() const { return !mort; }
	virtual void afficher(sf::RenderWindow& window);
	virtual void seDeplacer(bool useSatiete = true);
	unsigned int getAge() const { return age; }
	unsigned int getTempsAvantReproduction() const { return tempsAvantReproduction; }
	void setTempsAvantReproduction(unsigned int temps) { tempsAvantReproduction = temps; }
	virtual TypeEntite getType() const = 0;
	Sexe getSexe() const { return sexe; }
};

