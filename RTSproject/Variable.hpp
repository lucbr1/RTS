#pragma once
#include <SFML/Graphics.hpp>

inline unsigned int t = 1; // unité de temps en jour
inline unsigned int windowWidth = 1600; // Largeur de la fenêtre
inline unsigned int windowHeight = 900; // Hauteur de la fenêtre
inline unsigned int placeBoutton = 150; // place pour les boutons

//Grille
inline int cellSize = 5; // Taille des cellules de la grille
inline sf::Color gridColor = sf::Color(255, 255, 255); // Couleur des lignes de la grille


//Proies
inline unsigned int popInitialeH = 100; // population initiale de proies
inline double tauxReproductionH = 0.05; // taux de reproduction des proies
inline double tauxMortaliteH = 0.01; // taux de mortalité des proies
inline double tempsAvantReproductionH = 10; // temps avant reproduction des proies

//Prédateurs
inline unsigned int popInitialeC = 20; // population initiale de prédateurs
inline double tauxReproductionC = 0.02; // taux de reproduction des prédateurs
inline double tauxMortaliteC = 0.05; // taux de mortalité des prédateurs
inline double tempsAvantReproductionC = 10; // temps avant reproduction des prédateurs

//Animations
inline float dureeAnimation = 1.5f; // durée des animations en secondes
inline float tailleAnimation = 40.f; // taille minimale des animations en pixels (les cellules peuvent être petites)
inline bool playAnimations = true; // jouer les animations ou non

enum TypeAnimation
{
	MortNaturelle,      // loup mort de faim
	CombatPredateur,    // loup tué par un autre loup
	ProieMangee,        // biche mangée par un loup
	NaissanceProie,     // naissance d'un faon
	NaissancePredateur  // naissance d'un louveteau
};

enum Etape {
	PROIE_SEUL,
	PREDATEUR_SEUL,
	PROIE_PREDATEUR
};

enum TypeEntite
{
	Proie,
	Predateur
};
enum Sexe
{
	Male,
	Femelle
};