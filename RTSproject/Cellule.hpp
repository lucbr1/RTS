#pragma once
#include "Proie.hpp"
#include "Predateur.hpp"
#include "Variable.hpp"
#include "Entite.hpp"


class CCellule {
public:
	std::vector<CProie*> proies;
	std::vector<CPredateur*> predateurs;
	CProie* proieAffichee = nullptr;
	CPredateur* predateurAffiche = nullptr;

	void ajouter(CEntite* entite)
	{
		if (entite->getType() == TypeEntite::Proie)
		{
			auto* p = static_cast<CProie*>(entite);
			proies.push_back(p);
			if (!proieAffichee) 
				proieAffichee = p;
		}
		else
		{
			auto* pr = static_cast<CPredateur*>(entite);
			predateurs.push_back(pr);
			if (!predateurAffiche) 
				predateurAffiche = pr;
		}
	}

	void vider()
	{
		viderpredateurs();
		viderproies();
	}

	void viderproies()
	{
		proies.clear();
		proieAffichee = nullptr;
	}
	void viderpredateurs()
	{
		predateurs.clear();
		predateurAffiche = nullptr;
	}
};