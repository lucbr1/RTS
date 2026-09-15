#include "fonctions.hpp"


static std::mt19937& global_rng() {
	static std::mt19937 rng{ std::random_device{}() };
	return rng;
}

int randomizer(int min, int max) {
	std::uniform_int_distribution<> dis(min, max);
	return dis(global_rng());
}

void gameLoopProie(sf::RenderWindow& window, std::vector<CProie*>& proies) {
	for (int i = proies.size() - 1; i >= 0; --i) {
		int nbReproductions = proies[i]->seDeplacer();
		//suppression des proies mortes
		/*if (!proies[i]->estVivant()) {
			delete proies[i];
			proies.erase(proies.begin() + i);
			continue;
		}*/
		for (size_t j = 0; j < nbReproductions; ++j) {
			proies.push_back(new CProie());
		}
	}


}