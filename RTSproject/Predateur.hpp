#pragma once

#include "Entite.hpp"

class CPredateur : public CEntite
{
	unsigned int satiete;

	const sf::Texture& getTexture() const override;

public:
	CPredateur(int x, int y, unsigned int age = 0);
	void seDeplacer() override;
	unsigned int getSatiete() const { return satiete; }
	void setSatiete(unsigned int s) { satiete = s; }
};
