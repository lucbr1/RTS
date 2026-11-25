#pragma once
#include <vector>
#include "Proie.hpp"
#include "Predateur.hpp"

// seule la première entité de chaque liste est affichée
struct CCellule {
	std::vector<CProie*> proies;
	std::vector<CPredateur*> predateurs;
};
