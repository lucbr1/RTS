#include "Grille.hpp"
#include "Variable.hpp"
#include "fonctions.hpp"
#include "Proie.hpp"
#include "Predateur.hpp"
#include <iostream>

CGrille::CGrille(sf::RenderWindow& window) : window(window) {
	//calcule du nombre de ligne pour couvrir la fenetre et du nombre de vertex pour la grille
	const unsigned int numLinesX = (windowWidth / cellSize) + 1;
	const unsigned int numLinesY = (windowHeight / cellSize) + 1;
	const unsigned int totalVertices = (numLinesX + numLinesY) * 2;

	//création d'un tableau de vertex pour la grille
	sf::VertexArray grid(sf::PrimitiveType::Lines, totalVertices);
	size_t vertexIndex = 0;

	//génération des lignes verticales
	for (size_t i = 0; i < numLinesX; ++i)
	{
		float posX = i * cellSize;
		grid[vertexIndex++] = sf::Vertex{ { posX, 0.f }, gridColor };
		grid[vertexIndex++] = sf::Vertex{ { posX, float(windowHeight) }, gridColor };
	}

	//génération des lignes horizontales
	for (size_t j = 0; j < numLinesY; ++j)
	{
		float posY = j * cellSize;
		grid[vertexIndex++] = sf::Vertex{ { 0.f, posY }, gridColor };
		grid[vertexIndex++] = sf::Vertex{ { float(windowWidth), posY }, gridColor };
	}
	vertexGrille = grid;

	grille.resize(numLinesY);
	for (auto& ligne : grille)
	{
		ligne.resize(numLinesX);
	}

	
	//creation des proies
	for (size_t i = 0; i < popInitialeH; ++i) {
		int x = randomizer(0, numLinesX - 2);
		int y = randomizer(0, numLinesY - 2);
		grille[y][x].ajouter(new CProie(x, y, 30));
	}
	nbProies = popInitialeH;

	etape = Etape::PROIE_REPRODUCTION_SUR_CASE;
}

void CGrille::reset()
{
	// Supprimer toutes les entités
	for (auto& ligne : grille)
	{
		for (auto& cellule : ligne)
		{
			for (auto proie : cellule.proies)
			{
				delete proie;
			}
			for (auto predateur : cellule.predateurs)
			{
				delete predateur;
			}
			nbProies = 0;
			nbPredateurs = 0;
			cellule.vider();

		}
	}
	if (etape == Etape::PREDATEUR_SEUL_MORT_ALEATOIRE || etape == Etape::PREDATEUR_SEUL_MORT_BASTON)
	{
		// Recréer les prédateurs initiaux
		for (size_t i = 0; i < popInitialeC; ++i)
		{
			int x = randomizer(0, grille[0].size() - 2);
			int y = randomizer(0, grille.size() - 2);
			grille[y][x].ajouter(new CPredateur(x, y, 30));
		}
		nbPredateurs = popInitialeC;
	}
	else
	{
		// Recréer les proies initiales
		for (size_t i = 0; i < popInitialeH; ++i)
		{
			int x = randomizer(0, grille[0].size() - 2);
			int y = randomizer(0, grille.size() - 2);
			grille[y][x].ajouter(new CProie(x, y, 30));
		}
		nbProies = popInitialeH;
	}
}

void CGrille::afficher() const {
	window.draw(vertexGrille);
	for (const auto& ligne : grille)
	{
		for (const auto& cellule : ligne)
		{
			if (cellule.proieAffichee)
				cellule.proieAffichee->afficher(window);

			if (cellule.predateurAffiche)
				cellule.predateurAffiche->afficher(window);
		}
	}
}


void CGrille::gameLoop()
{
	if (etape == Etape::PROIE_REPRODUCTION_SUR_CASE || etape == Etape::PROIE_REPRODUCTION_ALEATOIRE)
		gameLoopProie();
	else if (etape == Etape::PREDATEUR_SEUL_MORT_ALEATOIRE || etape == Etape::PREDATEUR_SEUL_MORT_BASTON)
		gameLoopPredateur();
}
void CGrille::gameLoopProie()
{
	// 1. Déplacement des proies
	for (auto& ligne : grille)
		for (auto& cellule : ligne)
			for (auto proie : cellule.proies)
				proie->seDeplacer();

	// 2. Replacer les proies dans les bonnes cellules
	std::vector<CProie*> proies;
	proies.reserve(nbProies);

	for (auto& ligne : grille)
	{
		for (auto& cellule : ligne)
		{
			for (auto proie : cellule.proies)
				proies.push_back(proie);
			cellule.vider();
		}
	}

	// 3. Replacer chaque entité selon sa nouvelle position
	for (auto proie : proies)
	{
		int x = static_cast<int>(proie->getPosition().x / cellSize);
		int y = static_cast<int>(proie->getPosition().y / cellSize);

		grille[y][x].ajouter(proie);
	}
	Reproduction();
}



void CGrille::Reproduction() {
	std::vector<CEntite*> reproducteurs;
	for (size_t y = 0; y < grille.size(); ++y)
	{
		for (size_t x = 0; x < grille[y].size(); ++x)
		{
			auto& cellule = grille[y][x];

			reproducteurs.clear();
			for (auto proie : cellule.proies)
			{
				if (proie->getAge() >= 30 && proie->getTempsAvantReproduction() == 0)
					reproducteurs.push_back(proie);
			}

			unsigned int nbProie = reproducteurs.size() / 2;
			size_t idx = 0;
			for (size_t i = 0; i < nbProie; ++i)
			{
				if (etape == PROIE_REPRODUCTION_ALEATOIRE) {
					int newX = randomizer(0, grille[0].size() - 2);
					int newY = randomizer(0, grille.size() - 2);
					grille[newY][newX].ajouter(new CProie(newX, newY));
				}
				else {
					cellule.ajouter(new CProie((int)x, (int)y));
				}
				nbProies++;
				reproducteurs[idx++]->setTempsAvantReproduction(tempsAvantReproductionH);
				reproducteurs[idx++]->setTempsAvantReproduction(tempsAvantReproductionH);
			}
		}
	}
}

void CGrille::gameLoopPredateur()
{
	// 1. Déplacement des prédateurs
	for (auto& ligne : grille)
		for (auto& cellule : ligne)
			for (auto predateur : cellule.predateurs)
					predateur->seDeplacer();

	// 2. Replacer les entités dans les bonnes cellules
	std::vector<CPredateur*> predateurs;
	predateurs.reserve(nbPredateurs);

	for (auto& ligne : grille)
	{
		for (auto& cellule : ligne)
		{
			for (auto predateur : cellule.predateurs)
			{
				if(!( !predateur->estVivant()))
					predateurs.push_back(predateur);
				if (!predateur->estVivant())
					nbPredateurs--;
			}
			cellule.vider();
		}
	}
	// 3. Replacer chaque entité selon sa nouvelle position
	for (auto predateur : predateurs)
	{
		int x = static_cast<int>(predateur->getPosition().x / cellSize);
		int y = static_cast<int>(predateur->getPosition().y / cellSize);
		grille[y][x].ajouter(predateur);
	}
}