#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "Variable.hpp"

class CEntite {
protected:
	sf::Vector2f position;
	unsigned int age;
	CauseMort causeMort = CauseMort::Aucune;
	unsigned int tempsAvantReproduction = 0;
	Sexe sexe;

	virtual const sf::Texture& getTexture() const = 0;

public:
	CEntite(int x, int y, unsigned int age = 0);
	virtual ~CEntite() = default;

	void afficher(sf::RenderWindow& window) const;
	virtual void seDeplacer();

	sf::Vector2f getPosition() const { return position; }
	bool estVivant() const { return causeMort == CauseMort::Aucune; }
	CauseMort getCauseMort() const { return causeMort; }
	unsigned int getAge() const { return age; }
	Sexe getSexe() const { return sexe; }
	bool peutSeReproduire() const { return age >= ageMinReproduction && tempsAvantReproduction == 0; }
	void setTempsAvantReproduction(unsigned int temps) { tempsAvantReproduction = temps; }
};

sf::Texture chargerTexture(const std::string& chemin);
