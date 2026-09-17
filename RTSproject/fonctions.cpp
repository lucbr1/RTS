#include "fonctions.hpp"


static std::mt19937& global_rng() {
	static std::mt19937 rng{ std::random_device{}() };
	return rng;
}

int randomizer(int min, int max) {
	std::uniform_int_distribution<> dis(min, max);
	return dis(global_rng());
}