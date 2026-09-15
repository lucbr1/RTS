#include "fonctions.hpp"

int randomizer(int min, int max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(min, max);
	return dis(gen);
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