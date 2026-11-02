#include "fonctions.hpp"
#include <random>

static uint64_t initSeed() {
	std::random_device rd;
	uint64_t seed = (static_cast<uint64_t>(rd()) << 32) | rd();
	return seed ? seed : 0x9E3779B97F4A7C15ull; // xorshift ne doit jamais valoir 0
}

uint64_t rngState = initSeed();

int randomizer(int min, int max) {
	return min + static_cast<int>(randomBelow(static_cast<uint32_t>(max - min + 1)));
}
