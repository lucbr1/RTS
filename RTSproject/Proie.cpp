#include "Proie.hpp"

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
			mort = true;
			return;
		}
		satiete--;
	}
	CEntite::seDeplacer();
}
