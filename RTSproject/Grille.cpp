#include "Grille.hpp"
#include "Variable.hpp"
#include "fonctions.hpp"
#include "Proie.hpp"
#include "Predateur.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdint>

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
	
	reset();
}

void CGrille::reset()
{
	animations.clear();
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
	if (spawnC)
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
	if (spawnH)
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
	if (afficherGrille)
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

void CGrille::ajouterAnimation(TypeAnimation type, sf::Vector2f position)
{
	animations.push_back({ type, position, horloge.getElapsedTime().asSeconds() });
}

void CGrille::afficherAnimations(const sf::Font& font)
{
	if (!playAnimations)
		return;

	float maintenant = horloge.getElapsedTime().asSeconds();

	// suppression des animations terminées
	animations.erase(std::remove_if(animations.begin(), animations.end(),
		[&](const Animation& a) { return maintenant - a.debut >= dureeAnimation; }),
		animations.end());

	// taille de l'animation : au moins tailleAnimation pixels pour rester visible si les cellules sont petites
	float taille = std::max(static_cast<float>(cellSize), tailleAnimation);

	for (const auto& animation : animations)
	{
		float avancement = (maintenant - animation.debut) / dureeAnimation; // entre 0 et 1
		std::uint8_t alpha = static_cast<std::uint8_t>(255 * (1.f - avancement));
		sf::Vector2f centre = animation.position + sf::Vector2f(cellSize / 2.f, cellSize / 2.f);

		sf::Text texte(font, "", 14);
		texte.setStyle(sf::Text::Bold);
		texte.setOutlineColor(sf::Color(0, 0, 0, alpha));
		texte.setOutlineThickness(2.f);

		switch (animation.type)
		{
		case MortNaturelle:
		{
			// le loup devient gris, rétrécit et disparait
			float rayon = (taille / 4.f) * (1.f - avancement);
			sf::CircleShape loup(rayon);
			loup.setOrigin({ rayon, rayon });
			loup.setPosition(centre);
			loup.setFillColor(sf::Color(120, 120, 120, alpha));
			window.draw(loup);

			// une croix (tombe) qui monte
			sf::Vector2f posCroix = centre - sf::Vector2f(0.f, 15.f * avancement);
			sf::RectangleShape vertical({ 4.f, 18.f });
			vertical.setOrigin({ 2.f, 9.f });
			vertical.setPosition(posCroix);
			vertical.setFillColor(sf::Color(255, 255, 255, alpha));
			sf::RectangleShape horizontal({ 12.f, 4.f });
			horizontal.setOrigin({ 6.f, 7.f });
			horizontal.setPosition(posCroix);
			horizontal.setFillColor(sf::Color(255, 255, 255, alpha));
			window.draw(vertical);
			window.draw(horizontal);

			texte.setString(L"loup mort de faim");
			texte.setFillColor(sf::Color(200, 200, 200, alpha));
			break;
		}
		case CombatPredateur:
		{
			// deux loups qui se rentrent dedans plusieurs fois
			float rayon = taille / 6.f;
			float ecart = (taille / 3.f) * (1.f - std::abs(std::sin(avancement * 3.14159f * 3.f)));

			sf::CircleShape gagnant(rayon);
			gagnant.setOrigin({ rayon, rayon });
			gagnant.setPosition(centre - sf::Vector2f(ecart, 0.f));
			gagnant.setFillColor(sf::Color(255, 0, 0, alpha));

			// le perdant noircit au fil du combat
			sf::CircleShape perdant(rayon);
			perdant.setOrigin({ rayon, rayon });
			perdant.setPosition(centre + sf::Vector2f(ecart, 0.f));
			perdant.setFillColor(sf::Color(static_cast<std::uint8_t>(255 * (1.f - avancement)), 0, 0, alpha));

			// onde de choc orange
			float rayonOnde = (taille / 2.f) * avancement;
			sf::CircleShape onde(rayonOnde);
			onde.setOrigin({ rayonOnde, rayonOnde });
			onde.setPosition(centre);
			onde.setFillColor(sf::Color::Transparent);
			onde.setOutlineThickness(3.f);
			onde.setOutlineColor(sf::Color(255, 140, 0, alpha));

			window.draw(onde);
			window.draw(gagnant);
			window.draw(perdant);

			texte.setString(L"combat de loups");
			texte.setFillColor(sf::Color(255, 140, 0, alpha));
			break;
		}
		case ProieMangee:
		{
			// la biche passe du vert au rouge et rétrécit
			float rayon = (taille / 3.f) * (1.f - avancement);
			sf::CircleShape biche(rayon);
			biche.setOrigin({ rayon, rayon });
			biche.setPosition(centre);
			biche.setFillColor(sf::Color(
				static_cast<std::uint8_t>(150 * avancement),
				static_cast<std::uint8_t>(255 * (1.f - avancement)),
				0, alpha));

			// tache de sang qui s'étale
			float rayonSang = taille / 3.f + (taille / 3.f) * avancement;
			sf::CircleShape sang(rayonSang);
			sang.setOrigin({ rayonSang, rayonSang });
			sang.setPosition(centre);
			sang.setFillColor(sf::Color(150, 0, 0, static_cast<std::uint8_t>(alpha / 2)));

			window.draw(sang);
			window.draw(biche);

			texte.setString(L"biche mangée");
			texte.setFillColor(sf::Color(255, 80, 80, alpha));
			break;
		}
		case NaissanceProie:
		case NaissancePredateur:
		{
			// un coeur rose qui monte et grossit
			float r = (taille / 8.f) * (0.5f + avancement);
			sf::Vector2f posCoeur = centre - sf::Vector2f(0.f, taille / 4.f + 15.f * avancement);
			sf::Color rose(255, 105, 180, alpha);

			sf::CircleShape gauche(r);
			gauche.setOrigin({ r, r });
			gauche.setPosition(posCoeur + sf::Vector2f(-r * 0.7f, 0.f));
			gauche.setFillColor(rose);

			sf::CircleShape droite(r);
			droite.setOrigin({ r, r });
			droite.setPosition(posCoeur + sf::Vector2f(r * 0.7f, 0.f));
			droite.setFillColor(rose);

			sf::RectangleShape pointe({ r * 1.7f, r * 1.7f });
			pointe.setOrigin({ r * 0.85f, r * 0.85f });
			pointe.setPosition(posCoeur + sf::Vector2f(0.f, r * 0.7f));
			pointe.setRotation(sf::degrees(45.f));
			pointe.setFillColor(rose);

			window.draw(gauche);
			window.draw(droite);
			window.draw(pointe);

			// le petit qui apparait en grossissant (vert pour le faon, rouge pour le louveteau)
			float rayonBebe = (taille / 6.f) * avancement;
			sf::CircleShape bebe(rayonBebe);
			bebe.setOrigin({ rayonBebe, rayonBebe });
			bebe.setPosition(centre);
			bebe.setOutlineThickness(2.f);
			bebe.setOutlineColor(sf::Color(255, 255, 255, alpha));

			if (animation.type == NaissanceProie)
			{
				bebe.setFillColor(sf::Color(0, 255, 0, alpha));
				texte.setString(L"naissance d'un faon");
				texte.setFillColor(sf::Color(150, 255, 150, alpha));
			}
			else
			{
				bebe.setFillColor(sf::Color(255, 0, 0, alpha));
				texte.setString(L"naissance d'un louveteau");
				texte.setFillColor(sf::Color(255, 150, 200, alpha));
			}
			window.draw(bebe);
			break;
		}
		}

		// texte au dessus de la cellule qui monte doucement
		texte.setPosition(centre - sf::Vector2f(texte.getLocalBounds().size.x / 2.f, taille / 2.f + 12.f + 10.f * avancement));
		window.draw(texte);
	}
}


void CGrille::gameLoop()
{
	if (spawnC && spawnH)
		gameLoopProiePredateur();
	else if (spawnC)
		gameLoopPredateur();
	else if (spawnH)
		gameLoopProie();
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
				ajouterAnimation(NaissanceProie, sf::Vector2f(static_cast<float>(x * cellSize), static_cast<float>(y * cellSize)));

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
			if (cellule.predateurs.size() > 1)
				ajouterAnimation(CombatPredateur, cellule.predateurs[0]->getPosition());
			CEntite* predateur = cellule.predateurs[randomizer(0, cellule.predateurs.size() - 1)];
			nbPredateurs -= cellule.predateurs.size()-1;
			cellule.viderpredateurs();
			cellule.ajouter(predateur);
		}
	}
}


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
				else
				{
					ajouterAnimation(MortNaturelle, predateur->getPosition());
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

			// plusieurs mâles dans la cellule : un seul survit au combat
			if (males.size() > 1)
				ajouterAnimation(CombatPredateur, males[0]->getPosition());

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
					ajouterAnimation(NaissancePredateur, male->getPosition());

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

						ajouterAnimation(ProieMangee, cellule.proies[index]->getPosition());

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