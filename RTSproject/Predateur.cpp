#include "Predateur.hpp"
#include "fonctions.hpp"

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
	if (satiete == 0 && meurtDeFaimC)
	{
		causeMort = CauseMort::Faim;
		return;
	}
	if (satiete > 0)
		satiete--;

	if (mortNaturelleC && randomReel() < tauxMortaliteC)
	{
		causeMort = CauseMort::Naturelle;
		return;
	}
	CEntite::seDeplacer();
}
