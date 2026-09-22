#pragma once
#include <SFML/Graphics.hpp>

inline unsigned int t = 1; // unité de temps en jour
inline unsigned int windowWidth = 1600; // Largeur de la fenêtre
inline unsigned int windowHeight = 900; // Hauteur de la fenêtre
inline unsigned int placeBoutton = 150; // place pour les boutons

//Grille
inline int cellSize = 50; // Taille des cellules de la grille
inline sf::Color gridColor = sf::Color(255, 255, 255); // Couleur des lignes de la grille


//Proies
inline unsigned int popInitialeH = 10; // population initiale de proies
inline double tauxReproductionH = 0.05; // taux de reproduction des proies
inline double tauxMortaliteH = 0.01; // taux de mortalité des proies
inline double tempsAvantReproductionH = 10; // temps avant reproduction des proies

//Prédateurs
inline unsigned int popInitialeC = 100000; // population initiale de prédateurs
inline double tauxReproductionC = 0.02; // taux de reproduction des prédateurs
inline double tauxMortaliteC = 0.05; // taux de mortalité des prédateurs
inline double tempsAvantReproductionC = 10; // temps avant reproduction des prédateurs

enum Etape {
	PROIE_REPRODUCTION_SUR_CASE,
	PROIE_REPRODUCTION_ALEATOIRE,
	PREDATEUR_SEUL_MORT_ALEATOIRE,
	PREDATEUR_SEUL_MORT_BASTON
};

enum TypeEntite
{
	Proie,
	Predateur
};