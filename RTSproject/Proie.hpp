#pragma once

#include <SFML/Graphics.hpp>
#include "Variable.hpp"
#include "Entite.hpp"

class CProie : public CEntite 
{
	unsigned int satiete;
public:
	CProie(int x, int y, unsigned int age=0);
	void seDeplacer() override;
	void afficher(sf::RenderWindow& window) override;
	TypeEntite getType() const override { return TypeEntite::Proie; }
	unsigned int getSatiete() const { return satiete; }
	void setSatiete(unsigned int s) {satiete = s;}
};