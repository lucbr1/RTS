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

	etape = Etape::PROIE_SEUL;
}

void CGrille::reset()
{
	// Supprimer toutes les entités
	for (auto& ligne : grille)
	{
		for (auto& cellule : ligne)
		{
			nbProies = 0;
			nbPredateurs = 0;
			cellule.vider();

		}
	}
	if (etape == Etape::PREDATEUR_SEUL || etape == Etape::PROIE_PREDATEUR)
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
	if (etape == Etape::PROIE_SEUL || etape == Etape::PROIE_PREDATEUR)
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
	switch (etape) {
	case Etape::PROIE_PREDATEUR:
		gameLoopProiePredateur();
		break;
	case Etape::PROIE_SEUL:
		gameLoopProie();
		break;
	case Etape::PREDATEUR_SEUL:
		gameLoopPredateur();
		break;
	}
}

void CGrille::deplacementProies(bool useSatiete)
{
	// 1. Déplacement des proies
	for (auto& ligne : grille)
		for (auto& cellule : ligne)
			for (auto proie : cellule.proies)
				proie->seDeplacer(useSatiete);

	// 2. Replacer les proies dans les bonnes cellules
	std::vector<CProie*> proies;
	proies.reserve(nbProies);

	for (auto& ligne : grille)
	{
		for (auto& cellule : ligne)
		{
			for (auto proie : cellule.proies)
				proies.push_back(proie);
			cellule.viderproies();
		}
	}

	// 3. Replacer chaque entité selon sa nouvelle position
	for (auto proie : proies)
	{
		int x = static_cast<int>(proie->getPosition().x / cellSize);
		int y = static_cast<int>(proie->getPosition().y / cellSize);

		grille[y][x].ajouter(proie);
	}
}
void CGrille::gameLoopProie()
{
	deplacementProies(false);
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
				cellule.ajouter(new CProie((int)x, (int)y));

				nbProies++;
				reproducteurs[idx++]->setTempsAvantReproduction(tempsAvantReproductionH);
				reproducteurs[idx++]->setTempsAvantReproduction(tempsAvantReproductionH);
			}
		}
	}
}

void CGrille::deplacementPredateurs(bool useSatiete)
{
	// 1. Déplacement des prédateurs
	for (auto& ligne : grille)
		for (auto& cellule : ligne)
			for (auto predateur : cellule.predateurs)
				predateur->seDeplacer(useSatiete);
	// 2. Replacer les entités dans les bonnes cellules
	std::vector<CPredateur*> predateurs;
	predateurs.reserve(nbPredateurs);
	for (auto& ligne : grille)
	{
		for (auto& cellule : ligne)
		{
			for (auto predateur : cellule.predateurs)
				predateurs.push_back(predateur);
			cellule.viderpredateurs();
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

void CGrille::gameLoopPredateur()
{
	deplacementPredateurs(false);
	//tue tous les prédateurs sauf un par cellule
	for (auto& ligne : grille)
	{
		for (auto& cellule : ligne)
		{
			if (cellule.predateurs.size() == 0)
				continue;
			CEntite* predateur = cellule.predateurs[randomizer(0, cellule.predateurs.size() - 1)];
			nbPredateurs -= cellule.predateurs.size()-1;
			cellule.viderpredateurs();
			cellule.ajouter(predateur);
		}
	}
}

/*
void CGrille::gameLoopProiePredateur()
{
	deplacementPredateurs(true);
	deplacementProies(false);
	for (auto& ligne : grille)
		for (auto& cellule : ligne)
		{
			//gestion de la reproduction des prédateurs
			std::vector<CPredateur*> Females;
			std::vector<CPredateur*> Males;
			for (auto predateur : cellule.predateurs)
			{
				if (!predateur->estVivant())
				{
					nbPredateurs--;
					continue;
				}
				if (predateur->getSexe() == Sexe::Male)
				{
					Males.push_back(predateur);
				}
				else
				{
					Females.push_back(predateur);
				}
			}
			//reproduction des prédateurs
			if (Males.size() > 0 && Females.size() > 0)
			{
				CPredateur* Male = Males[randomizer(0, Males.size() - 1)];
				CPredateur Female = *Females[randomizer(0, Females.size() - 1)];

				if (Male->getAge() >= 30 && Male->getTempsAvantReproduction() == 0 && Female.getAge() >= 30 && Female.getTempsAvantReproduction() == 0)
				{
					cellule.ajouter(new CPredateur(static_cast<int>(cellule.predateurs[0]->getPosition().x / cellSize), static_cast<int>(cellule.predateurs[0]->getPosition().y / cellSize)));
					nbPredateurs++;
					Male->setTempsAvantReproduction(tempsAvantReproductionC);
					Female.setTempsAvantReproduction(tempsAvantReproductionC);
				}
				nbPredateurs -= Males.size() - 1;
				cellule.predateurs = Females;
				cellule.predateurAffiche = Females.empty() ? nullptr : Females[0];

				cellule.ajouter(Male);
			}
			else
			{
				if (Males.size() > 0)
				{
					nbPredateurs -= Males.size() - 1;
					cellule.viderpredateurs();
					cellule.ajouter(Males[0]);
				}
				else
				{
					cellule.predateurs = Females;
					cellule.predateurAffiche = Females.empty() ? nullptr : Females[0];
				}
			}

			// mort des proies par les prédateurs
			if (cellule.predateurs.size() > 0 && cellule.proies.size() > 0)
			{
				std::vector<CProie*> proies= cellule.proies;
				for(auto predateur : cellule.predateurs)
				{
					if (predateur->getSatiete() <5)
					{
						proies.erase(proies.begin()+ randomizer(0, proies.size() - 1));
						nbProies--;
						predateur->setSatiete(10);
					}
					if (proies.size() == 0)
					{
						break;
					}
				}
				cellule.proies = proies;
			}
		}

}*/


void CGrille::gameLoopProiePredateur()
{
	nbPredateurs = 0;
	nbProies = 0;
	// Déplacement des prédateurs et des proies
	deplacementPredateurs(true);
	deplacementProies(false);

	for (auto& ligne : grille)
	{
		for (auto& cellule : ligne)
		{
			std::vector<CPredateur*> predateursVivants;

			for (auto predateur : cellule.predateurs)
			{
				if (predateur->estVivant())
				{
					predateursVivants.push_back(predateur);
				}
			}

			cellule.predateurs = predateursVivants;

			cellule.predateurAffiche = cellule.predateurs.empty() ? nullptr : cellule.predateurs[0];

			std::vector<CPredateur*> males;
			std::vector<CPredateur*> femelles;

			for (auto predateur : cellule.predateurs)
			{
				if (predateur->getSexe() == Sexe::Male)
					males.push_back(predateur);
				else
					femelles.push_back(predateur);
			}
			cellule.viderpredateurs();

			if (!males.empty() && !femelles.empty())
			{
				CPredateur* male = males[randomizer(0, static_cast<int>(males.size()) - 1)];

				CPredateur* femelle = femelles[randomizer(0, static_cast<int>(femelles.size()) - 1)];

				
				for (auto femelle : femelles)
				{
					cellule.ajouter(femelle);
				}
				cellule.ajouter(male);

				if (male->getAge() >= 30 && male->getTempsAvantReproduction() == 0 &&
					femelle->getAge() >= 30 && femelle->getTempsAvantReproduction() == 0)
				{
					int x = static_cast<int>(male->getPosition().x / cellSize);
					int y = static_cast<int>(male->getPosition().y / cellSize);

					cellule.ajouter(new CPredateur(x, y));

					male->setTempsAvantReproduction(tempsAvantReproductionC);
					femelle->setTempsAvantReproduction(tempsAvantReproductionC);
				}
			}
			else
			{
				if (!males.empty())
				{
					CPredateur* maleGagnant = males[randomizer(0, static_cast<int>(males.size()) - 1)];
					cellule.ajouter(maleGagnant);
				}
				if (!femelles.empty())
				{
					for (auto femelle : femelles)
					{
						cellule.ajouter(femelle);
					}
				}
			}


			if (!cellule.predateurs.empty() && !cellule.proies.empty())
			{
				for (auto predateur : cellule.predateurs)
				{
					if (cellule.proies.empty())
						break;

					if (predateur->getSatiete() < 5)
					{
						int index = randomizer( 0, static_cast<int>(cellule.proies.size()) - 1 );

						cellule.proies.erase( cellule.proies.begin() + index );
						predateur->setSatiete(10);
					}
				}

				cellule.proieAffichee = cellule.proies.empty() ? nullptr : cellule.proies[0];
			}
			nbPredateurs += static_cast<unsigned int>(cellule.predateurs.size());

			nbProies += static_cast<unsigned int>(cellule.proies.size());
		}
	}
}