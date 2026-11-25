#pragma once
#include <SFML/Graphics.hpp>

inline unsigned int t = 1; // nombre de jours simulés par appui sur Espace
inline unsigned int windowWidth = 1600;
inline unsigned int windowHeight = 900;
inline unsigned int placeBoutton = 150; // largeur de la barre latérale

//Grille
inline int cellSize = 25;
inline bool afficherGrille = true;
inline sf::Color gridColor = sf::Color(255, 255, 255);

//Commun
inline unsigned int ageMinReproduction = 30; // jours

//Proies
inline bool spawnH = true;
inline unsigned int popInitialeH = 100;
inline double tauxReproductionH = 0.75;
inline double tauxMortaliteH = 0.1;
inline bool mortNaturelleH = true; // les proies peuvent mourir aléatoirement de vieillesse
inline bool estCeQueTempsAvantReproductionH = true;
inline unsigned int tempsAvantReproductionH = 10; // jours
inline bool meurtDeFaimH = false;
inline unsigned int tempsMortDeFaimH = 10; // jours sans manger avant de mourir

//Prédateurs
inline bool spawnC = true;
inline unsigned int popInitialeC = 200;
inline double tauxReproductionC = 0.02;
inline double tauxMortaliteC = 0.1;
inline bool mortNaturelleC = true; // les predateurs peuvent mourir aléatoirement de vieillesse
inline bool estCeQueTempsAvantReproductionC = true;
inline unsigned int tempsAvantReproductionC = 10; // jours
inline bool meurtDeFaimC = false;
inline unsigned int tempsMortDeFaimC = 10; // jours sans manger avant de mourir
inline bool seBatteC = true;

//Animations
inline float dureeAnimation = 1.5f; // secondes
inline float tailleAnimation = 40.f; // taille minimale en pixels (les cellules peuvent être petites)
inline bool playAnimations = true;

enum TypeAnimation
{
	MortDeFaimProie,
	MortDeFaimPredateur,
	MortNaturelleProie,
	MortNaturellePredateur,
	CombatPredateur,    // loup tué par un autre loup
	ProieMangee,        // chevreuil mangé par un loup
	NaissanceProie,     // naissance d'un faon
	NaissancePredateur  // naissance d'un louveteau
};

enum class CauseMort
{
	Aucune,
	Faim,
	Naturelle
};

enum Sexe
{
	Male,
	Femelle
};
