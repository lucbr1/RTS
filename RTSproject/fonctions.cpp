#include "fonctions.hpp"
#include <random>


static std::mt19937& global_rng() {
	static std::mt19937 rng{ std::random_device{}() };
	return rng;
}
int randomizer(int min, int max) {
	return min + static_cast<int>(global_rng()() % static_cast<unsigned int>(max - min + 1));
}
