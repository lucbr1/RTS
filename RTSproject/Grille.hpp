#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <vector>
#include "Variable.hpp"

// Structure of Arrays : une colonne par attribut, l'entité i est l'ensemble des colonnes à l'indice i.
// Les entités sont stockées en continu, sans pointeur ni allocation individuelle.
// La position est l'indice de cellule (y * largeur + x), donc la grille doit avoir moins de 65536 cellules.

// Une proie = 3 octets. L'âge de reproduction et le temps d'attente sont fusionnés en un seul compteur :
// nombre de pas restants avant de pouvoir se reproduire (0 = éligible).
struct SoaProies {
	std::vector<uint16_t> cell;
	std::vector<uint8_t> delai;

	size_t size() const { return cell.size(); }

	void vider()
	{
		cell.clear();
		delai.clear();
	}

	// Compacte en place en gardant les entités pour lesquelles garder(i) est vrai (ordre conservé).
	// garder est appelé une seule fois par indice, dans l'ordre croissant.
	template <class Garder>
	void filtrer(Garder garder)
	{
		size_t w = 0;
		for (size_t i = 0, n = size(); i < n; ++i)
		{
			if (!garder(i))
				continue;
			cell[w] = cell[i];
			delai[w] = delai[i];
			++w;
		}
		cell.resize(w);
		delai.resize(w);
	}
};

struct SoaPredateurs {
	std::vector<uint16_t> cell;
	std::vector<uint16_t> attente;
	std::vector<uint32_t> age;
	std::vector<uint8_t> satiete;
	std::vector<uint8_t> sexe;       // valeurs de l'enum Sexe

	size_t size() const { return cell.size(); }

	void ajouter(uint16_t c, uint32_t a, uint8_t s, uint8_t sx)
	{
		cell.push_back(c);
		attente.push_back(0);
		age.push_back(a);
		satiete.push_back(s);
		sexe.push_back(sx);
	}

	void vider()
	{
		cell.clear();
		attente.clear();
		age.clear();
		satiete.clear();
		sexe.clear();
	}

	template <class Garder>
	void filtrer(Garder garder)
	{
		size_t w = 0;
		for (size_t i = 0, n = size(); i < n; ++i)
		{
			if (!garder(i))
				continue;
			cell[w] = cell[i];
			attente[w] = attente[i];
			age[w] = age[i];
			satiete[w] = satiete[i];
			sexe[w] = sexe[i];
			++w;
		}
		cell.resize(w);
		attente.resize(w);
		age.resize(w);
		satiete.resize(w);
		sexe.resize(w);
	}
};

class CGrille {
	sf::VertexArray vertexGrille;
	sf::RenderWindow& window;
	sf::CircleShape formeProie;
	sf::CircleShape formePredateur;
	Etape etape;

	// dimensions de la grille en cellules
	uint32_t largeur;
	uint32_t hauteur;

	SoaProies proies;
	SoaPredateurs predateurs;
	std::vector<uint8_t> garder; // tampon réutilisé entre les pas de simulation

	// présence d'au moins une entité par cellule, recalculée seulement si la simulation a avancé
	std::vector<uint8_t> presenceProies;
	std::vector<uint8_t> presencePredateurs;
	bool presenceAJour = false;

	// pour chaque cellule, les 9 cellules atteignables en un pas (bords rebouclés) : évite le calcul x/y
	std::vector<uint16_t> voisins;

	uint32_t nbCellules() const { return largeur * hauteur; }

	void peuplerProies(size_t nb);
	void peuplerPredateurs(size_t nb);
	void majPresence();

	void deplacementProies(bool reproduire);
	void deplacementPredateurs(bool useSatiete);

public:
	CGrille(sf::RenderWindow& window);
	void afficher();
	void gameLoop();
	void gameLoopProie();
	void gameLoopPredateur();
	void gameLoopProiePredateur();

	void reset();
	void setEtape(Etape etape) { this->etape = etape; reset(); }
	unsigned long long getNbProies() const { return proies.size(); }
	unsigned long long getNbPredateurs() const { return predateurs.size(); }
};
