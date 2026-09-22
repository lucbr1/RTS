#pragma once

#include <SFML/Graphics.hpp>
#include "Variable.hpp"
#include "Entite.hpp"

class CPredateur : public CEntite
{
public:
	CPredateur(int x, int y, unsigned int age = 0);
	void afficher(sf::RenderWindow& window) override;
	void seDeplacer() override;
	TypeEntite getType() const override { return TypeEntite::Predateur; }
};