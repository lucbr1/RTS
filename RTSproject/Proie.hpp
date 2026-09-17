#pragma once

#include <SFML/Graphics.hpp>
#include "Variable.hpp"
#include "Entite.hpp"

class CProie : public CEntite 
{
public:
	CProie(int x, int y, unsigned int age=0);
	void afficher(sf::RenderWindow& window) override;
};