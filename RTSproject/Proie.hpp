#pragma once

#include "Entite.hpp"

class CProie : public CEntite
{
	unsigned int satiete;

	const sf::Texture& getTexture() const override;

public:
	CProie(int x, int y, unsigned int age = 0);
	void seDeplacer() override;
};
