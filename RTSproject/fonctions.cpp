#include "fonctions.hpp"
#include <random>


static std::mt19937& global_rng() {
	static std::mt19937 rng{ std::random_device{}() };
	return rng;
}
int randomizer(int min, int max) {
	return min + static_cast<int>(global_rng()() % static_cast<unsigned int>(max - min + 1));
}

double randomReel() {
	return std::uniform_real_distribution<double>(0.0, 1.0)(global_rng());
}

bool isDead(unsigned int age, double beta, double alpha)
{
    // Calcul de p(x) selon la loi de Gompertz
    double p = beta * std::exp(alpha * age);

    // Saturation à 1.0 pour éviter les probabilités > 100%
    if (p > 1.0) p = 1.0;

    // Tirage bernoulli via distribution uniforme
    return std::uniform_real_distribution<double>(0.0, 1.0)(global_rng()) < p;
}
