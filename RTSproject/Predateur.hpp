#pragma once

#include <SFML/Graphics.hpp>
#include "Variable.hpp"
#include "Entite.hpp"

class CPredateur : public CEntite
{
	unsigned int satiete = 10;
public:
	CPredateur(int x, int y, unsigned int age = 0);
	void afficher(sf::RenderWindow& window) override;
	void seDeplacer(bool useSatiete = true) override;
	TypeEntite getType() const override { return TypeEntite::Predateur; }
	unsigned int getSatiete() const { return satiete; }
	void setSatiete(unsigned int s) { satiete = s; }
};