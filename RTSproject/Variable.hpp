#pragma once
#include <SFML/Graphics.hpp>

inline unsigned int t = 1; // unité de temps en jour
inline unsigned int windowWidth = 1600; // Largeur de la fenêtre
inline unsigned int windowHeight = 900; // Hauteur de la fenêtre
inline unsigned int placeBoutton = 150; // place pour les boutons

//Grille
inline int cellSize = 25; // Taille des cellules de la grille
inline bool afficherGrille = true; // afficher les lignes de la grille ou non
inline sf::Color gridColor = sf::Color(255, 255, 255); // Couleur des lignes de la grille


//Proies
inline bool spawnH = true; // si les proies apparaissent ou non
inline unsigned int popInitialeH = 100; // population initiale de proies
inline double tauxReproductionH = 0.05; // taux de reproduction des proies
inline double tauxMortaliteH = 0.01; // taux de mortalité des proies
inline bool estCeQueTempsAvantReproductionH = true; // si les proies ont un temps avant reproduction ou non
inline unsigned int tempsAvantReproductionH = 10; // temps avant reproduction des proies (jours)
inline bool meurtDeFaimH = false; // si les proies meurent de faim ou non
inline unsigned int tempsMortDeFaimH = 10; // nombre de jours sans manger avant qu'une proie meure de faim

//Prédateurs
inline bool spawnC = true; // si les prédateurs apparaissent ou non
inline unsigned int popInitialeC = 200; // population initiale de prédateurs
inline double tauxReproductionC = 0.02; // taux de reproduction des prédateurs
inline double tauxMortaliteC = 0.05; // taux de mortalité des prédateurs
inline bool estCeQueTempsAvantReproductionC = true; // si les prédateurs ont un temps avant reproduction ou non
inline unsigned int tempsAvantReproductionC = 10; // temps avant reproduction des prédateurs (jours)
inline bool meurtDeFaimC = false; // si les prédateurs meurent de faim ou non
inline unsigned int tempsMortDeFaimC = 10; // nombre de jours sans manger avant qu'un prédateur meure de faim
inline bool seBatteC = true; // si les prédateurs se battent entre eux ou non

//Animations
inline float dureeAnimation = 1.5f; // durée des animations en secondes
inline float tailleAnimation = 40.f; // taille minimale des animations en pixels (les cellules peuvent être petites)
inline bool playAnimations = true; // jouer les animations ou non






enum TypeAnimation
{
	MortNaturelle,      // loup mort de faim
	CombatPredateur,    // loup tué par un autre loup
	ProieMangee,        // chevreuil mangé par un loup
	NaissanceProie,     // naissance d'un faon
	NaissancePredateur  // naissance d'un louveteau
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