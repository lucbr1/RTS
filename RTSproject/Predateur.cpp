#include "Predateur.hpp"

CPredateur::CPredateur(int x, int y, unsigned int age) : CEntite(x, y, age), satiete(tempsMortDeFaimC)
{
}

const sf::Texture& CPredateur::getTexture() const
{
	static const sf::Texture texture = chargerTexture("assets/loup.png");
	return texture;
}

void CPredateur::seDeplacer()
{
	if (meurtDeFaimC)
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
