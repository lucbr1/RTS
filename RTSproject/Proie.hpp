#pragma once

#include "Entite.hpp"

class CProie : public CEntite
{
	unsigned int satiete;

	const sf::Texture& getTexture() const override;

public:
	CProie(int x, int y, unsigned int age = 0);
	void seDeplacer() override;
	unsigned int getSatiete() const { return satiete; }
	void setSatiete(unsigned int s) { satiete = s; }
	bool peutSeReproduire() const { return age >= ageMinReproduction && tempsAvantReproduction == 0 
		&& (reproductionEnFonctionDeLaSatieteH ? getSatiete() > seuilSatieteReproductionH : true); }
};
