#include "Grille.hpp"
#include "Variable.hpp"
#include "fonctions.hpp"
#include  <iostream>
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
		grille[y][x].push_back(new CProie(x, y, 30));
	}
}


void CGrille::afficher() const {
	window.draw(vertexGrille);
	for (auto ligne : grille)
	{
		for (auto cellule : ligne)
		{
			for (auto entite : cellule)
			{
				if (dynamic_cast<CProie*>(entite))
				{
					entite->afficher(window);
					break;
				}
			}
		}
	}
}

void CGrille::gameLoopProie(Etape etape)
{
	// 1. Déplacement des proies
	for (auto& ligne : grille)
	{
		for (auto& cellule : ligne)
		{
			for (auto entite : cellule)
			{
				if (dynamic_cast<CProie*>(entite))
				{
					entite->seDeplacer();
				}
			}
		}
	}

	// 2. Replacer les entités dans les bonnes cellules
	std::vector<CEntite*> entites;

	for (auto& ligne : grille)
	{
		for (auto& cellule : ligne)
		{
			for (auto entite : cellule)
			{
				entites.push_back(entite);
			}

			cellule.clear();
		}
	}

	// 3. Replacer chaque entité selon sa nouvelle position
	for (auto entite : entites)
	{
		int x = static_cast<int>(entite->getPosition().x / cellSize);
		int y = static_cast<int>(entite->getPosition().y / cellSize);

		grille[y][x].push_back(entite);
	}
	Reproduction(etape);
}



void CGrille::Reproduction(Etape etape) {
	for (size_t y = 0; y < grille.size(); ++y)
	{
		for (size_t x = 0; x < grille[y].size(); ++x)
		{
			auto& cellule = grille[y][x];
			unsigned int nbProie = 0;
			for (auto entite : cellule)
			{
				if (dynamic_cast<CProie*>(entite) && entite->getAge() >= 30 && entite->getTempsAvantReproduction() == 0)
					nbProie++;
			}

			nbProie /= 2;
			for (size_t i = 0; i < nbProie; ++i) {
				if (etape == PROIE_REPRODUCTION_ALEATOIRE) {
					int newX = randomizer(0, grille[0].size() - 2);
					int newY = randomizer(0, grille.size() - 2);
					grille[newY][newX].push_back(new CProie(newX, newY));
				}
				else
					cellule.push_back(new CProie(x, y));
				int nbProieReset = 0;
				while (nbProieReset < 2) {
					for (size_t i = 0; i < cellule.size(); ++i) {
						if (dynamic_cast<CProie*>(cellule[i]) && cellule[i]->getAge() >= 30 && cellule[i]->getTempsAvantReproduction() == 0) {
							cellule[i]->setTempsAvantReproduction(tempsAvantReproductionH);
							nbProieReset++;
							break;
						}
					}
				}
			}
		}
	}
}

unsigned int CGrille::getNombreProies() const {
	unsigned int totalProies = 0;
	for (const auto& ligne : grille)
	{
		for (const auto& cellule : ligne)
		{
			for (const auto& entite : cellule)
			{
				if (dynamic_cast<CProie*>(entite))
				{
					totalProies++;
				}
			}
		}
	}
	return totalProies;
}