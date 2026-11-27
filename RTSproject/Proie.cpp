#include "Proie.hpp"
#include "fonctions.hpp"

CProie::CProie(int x, int y, unsigned int age) : CEntite(x, y, age), satiete(tempsMortDeFaimH)
{
}

const sf::Texture& CProie::getTexture() const
{
	static const sf::Texture texture = chargerTexture("assets/chevreuil.png");
	return texture;
}

void CProie::seDeplacer()
{
	if (meurtDeFaimH)
	{
		if (satiete == 0)
		{
			causeMort = CauseMort::Faim;
			return;
		}
		satiete--;
	}
	if (mortNaturelleH && isDead(age, mortaliteInitialeH, coefficientRisqueMortH))
	{
		causeMort = CauseMort::Naturelle;
		return;
	}
	CEntite::seDeplacer();
}
