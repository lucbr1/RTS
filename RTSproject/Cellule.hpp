#pragma once
#include <vector>
#include "Proie.hpp"
#include "Predateur.hpp"

// seule la première entité de chaque liste est affichée
struct CCellule {
	unsigned int quantiteHerbe = quantiteMaxHerbe; // quantité d'herbe dans la cellule (pleine au départ)
	unsigned int tempsAvantReapparitionHerbe = 0; // en jours (0 = l'herbe ne repousse pas en ce moment)
	std::vector<CProie*> proies;
	std::vector<CPredateur*> predateurs;
};
