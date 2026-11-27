#pragma once

int randomizer(int min, int max);

double randomReel(); // dans [0, 1)

// Retourne true si un individu d'âge "age" devrait mourir selon la loi de Gompertz avec les paramètres "beta" et "alpha"
bool isDead(unsigned int age, double beta, double alpha);
