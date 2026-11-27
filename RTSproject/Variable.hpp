#pragma once
#include <SFML/Graphics.hpp>

inline unsigned int t = 1; // nombre de jours simulés par appui sur Espace
inline unsigned int windowWidth = 1600;
inline unsigned int windowHeight = 900;
inline unsigned int placeBoutton = 150; // largeur de la barre latérale

//Grille
inline unsigned int cellSize = 25; // taille d'une case en pixels (modifiable dans le menu)
inline bool afficherGrille = true;
inline sf::Color gridColor = sf::Color(255, 255, 255);

//Commun
inline unsigned int ageMinReproduction = 30; // jours

//Proies
inline bool spawnH = true;
inline unsigned int popInitialeH = 800;
inline double tauxReproductionH = 0.75;
inline double tauxMortaliteH = 0.01;
inline bool mortNaturelleH = true; // les proies peuvent mourir aléatoirement de vieillesse
inline bool estCeQueTempsAvantReproductionH = true;
inline unsigned int tempsAvantReproductionH = 10; // jours
inline bool meurtDeFaimH = false;
inline unsigned int tempsMortDeFaimH = 10; // jours sans manger avant de mourir
inline bool reproductionEnFonctionDeLaSatieteH = true; // les proies se reproduisent en fonction de leur satiété
inline double seuilSatieteReproductionH = 5; // seuil de satiété pour la reproduction (0 à satiete)
inline double mortaliteInitialeH = 0.0005; // probabilité qu'une proie meure dès le début de la simulation (0 à 1)
inline double coefficientRisqueMortH = 0.00082; // coefficient de risque de mortalité (0 à 1) naturelle (vieillesse)

//Prédateurs
inline bool spawnC = true;
inline unsigned int popInitialeC = 100;
inline double tauxReproductionC = 0.30;
inline double tauxMortaliteC = 0.1;
inline bool mortNaturelleC = false; // les predateurs peuvent mourir aléatoirement de vieillesse
inline bool estCeQueTempsAvantReproductionC = false;
inline unsigned int tempsAvantReproductionC = 10; // jours
inline bool meurtDeFaimC = true;
inline unsigned int tempsMortDeFaimC = 40; // jours sans manger avant de mourir
inline bool seBatteC = false;
inline double probaCombat = 0.2; // probabilité qu'un mâle meure au combat (quand il y a plusieurs mâles dans la cellule)
inline bool reproductionEnFonctionDeLaSatieteC = true; // les predateurs se reproduisent en fonction de leur satiété
inline double seuilSatieteReproductionC = 5; // seuil de satiété pour la reproduction (0 à satiete)
inline double mortaliteInitialeC = 0.0003; // probabilité qu'une proie meure dès le début de la simulation (0 à 1)
inline double coefficientRisqueMortC = 0.00118; // coefficient de risque de mortalité (0 à 1) naturelle (vieillesse)

//Cellules
inline unsigned int quantiteMaxHerbe = 2; // quantité maximale d'herbe dans une cellule
inline unsigned int tempsAvantReapparitionHerbe = 5; // jours avant que l'herbe ne repousse

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
