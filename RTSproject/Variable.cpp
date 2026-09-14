#pragma once
#include <SFML/Graphics.hpp>


unsigned int t = 1; // unité de temps en jour
unsigned int windowWidth = 1600; // Largeur de la fenêtre
unsigned int windowHeight = 900; // Hauteur de la fenêtre

//Grille
float cellSize = 50.f; // Taille des cellules de la grille
sf::Color gridColor = sf::Color(255, 255, 255); // Couleur des lignes de la grille


//Proies
unsigned int popInitialeH = 2; // population initiale de proies
double tauxReproductionH = 0.05; // taux de reproduction des proies
double tauxMortaliteH = 0.01; // taux de mortalité des proies

//Prédateurs
unsigned int popInitialeC = 1; // population initiale de prédateurs
double tauxReproductionC = 0.02; // taux de reproduction des prédateurs
double tauxMortaliteC = 0.05; // taux de mortalité des prédateurs


