#include "Grille.hpp"
#include "fonctions.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

CGrille::CGrille(sf::RenderWindow& window) : window(window)
{
	const unsigned int nbColonnes = windowWidth / cellSize;
	const unsigned int nbLignes = windowHeight / cellSize;

	// nbColonnes + 1 lignes verticales et nbLignes + 1 lignes horizontales pour fermer les bords
	vertexGrille = sf::VertexArray(sf::PrimitiveType::Lines);
	for (unsigned int i = 0; i <= nbColonnes; ++i)
	{
		float x = static_cast<float>(i * cellSize);
		vertexGrille.append(sf::Vertex{ { x, 0.f }, gridColor });
		vertexGrille.append(sf::Vertex{ { x, float(windowHeight) }, gridColor });
	}
	for (unsigned int j = 0; j <= nbLignes; ++j)
	{
		float y = static_cast<float>(j * cellSize);
		vertexGrille.append(sf::Vertex{ { 0.f, y }, gridColor });
		vertexGrille.append(sf::Vertex{ { float(windowWidth), y }, gridColor });
	}

	grille.assign(nbLignes, std::vector<CCellule>(nbColonnes));
	reset();
}

CGrille::~CGrille()
{
	vider();
}

void CGrille::vider()
{
	for (auto& ligne : grille)
	{
		for (auto& cellule : ligne)
		{
			for (auto proie : cellule.proies)
				delete proie;
			for (auto predateur : cellule.predateurs)
				delete predateur;
			cellule.proies.clear();
			cellule.predateurs.clear();
		}
	}
}

void CGrille::reset()
{
	animations.clear();
	vider();

	const int xMax = static_cast<int>(grille[0].size()) - 1;
	const int yMax = static_cast<int>(grille.size()) - 1;

	if (spawnC)
	{
		for (unsigned int i = 0; i < popInitialeC; ++i)
		{
			int x = randomizer(0, xMax);
			int y = randomizer(0, yMax);
			grille[y][x].predateurs.push_back(new CPredateur(x, y, ageMinReproduction));
		}
	}
	if (spawnH)
	{
		for (unsigned int i = 0; i < popInitialeH; ++i)
		{
			int x = randomizer(0, xMax);
			int y = randomizer(0, yMax);
			grille[y][x].proies.push_back(new CProie(x, y, ageMinReproduction));
		}
	}
	compterPopulations();
}

void CGrille::afficher() const
{
	if (afficherGrille)
		window.draw(vertexGrille);
	for (const auto& ligne : grille)
	{
		for (const auto& cellule : ligne)
		{
			if (!cellule.proies.empty())
				cellule.proies.front()->afficher(window);
			if (!cellule.predateurs.empty())
				cellule.predateurs.front()->afficher(window);
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
			// la chevreuil passe du vert au rouge et rétrécit
			float rayon = (taille / 3.f) * (1.f - avancement);
			sf::CircleShape chevreuil(rayon);
			chevreuil.setOrigin({ rayon, rayon });
			chevreuil.setPosition(centre);
			chevreuil.setFillColor(sf::Color(
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
			window.draw(chevreuil);

			texte.setString(L"chevreuil mangé");
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
	compterPopulations();
}

void CGrille::compterPopulations()
{
	nbProies = 0;
	nbPredateurs = 0;
	for (const auto& ligne : grille)
	{
		for (const auto& cellule : ligne)
		{
			nbProies += static_cast<unsigned int>(cellule.proies.size());
			nbPredateurs += static_cast<unsigned int>(cellule.predateurs.size());
		}
	}
}

// déplace toutes les entités puis les range dans leur nouvelle cellule
void CGrille::deplacement()
{
	std::vector<CProie*> proies;
	std::vector<CPredateur*> predateurs;
	proies.reserve(nbProies);
	predateurs.reserve(nbPredateurs);

	for (auto& ligne : grille)
	{
		for (auto& cellule : ligne)
		{
			proies.insert(proies.end(), cellule.proies.begin(), cellule.proies.end());
			predateurs.insert(predateurs.end(), cellule.predateurs.begin(), cellule.predateurs.end());
			cellule.proies.clear();
			cellule.predateurs.clear();
		}
	}

	auto cellule = [this](const CEntite* e) -> CCellule& {
		return grille[static_cast<int>(e->getPosition().y / cellSize)][static_cast<int>(e->getPosition().x / cellSize)];
	};
	for (auto proie : proies)
	{
		proie->seDeplacer();
		cellule(proie).proies.push_back(proie);
	}
	for (auto predateur : predateurs)
	{
		predateur->seDeplacer();
		cellule(predateur).predateurs.push_back(predateur);
	}
}

void CGrille::retirerMorts(CCellule& cellule)
{
	auto& proies = cellule.proies;
	proies.erase(std::remove_if(proies.begin(), proies.end(),
		[](CProie* p) {
			if (p->estVivant())
				return false;
			delete p;
			return true;
		}), proies.end());

	auto& predateurs = cellule.predateurs;
	predateurs.erase(std::remove_if(predateurs.begin(), predateurs.end(),
		[this](CPredateur* p) {
			if (p->estVivant())
				return false;
			ajouterAnimation(MortNaturelle, p->getPosition());
			delete p;
			return true;
		}), predateurs.end());
}

// chaque paire de proies en âge de se reproduire a une probabilité tauxReproductionH de donner un faon
void CGrille::reproductionProies()
{
	std::vector<CProie*> reproducteurs;
	for (size_t y = 0; y < grille.size(); ++y)
	{
		for (size_t x = 0; x < grille[y].size(); ++x)
		{
			auto& cellule = grille[y][x];

			reproducteurs.clear();
			for (auto proie : cellule.proies)
				if (proie->peutSeReproduire())
					reproducteurs.push_back(proie);

			for (size_t i = 0; i + 1 < reproducteurs.size(); i += 2)
			{
				if (randomReel() >= tauxReproductionH)
					continue;

				cellule.proies.push_back(new CProie(static_cast<int>(x), static_cast<int>(y)));
				ajouterAnimation(NaissanceProie, reproducteurs[i]->getPosition());

				if (estCeQueTempsAvantReproductionH)
				{
					reproducteurs[i]->setTempsAvantReproduction(tempsAvantReproductionH);
					reproducteurs[i + 1]->setTempsAvantReproduction(tempsAvantReproductionH);
				}
			}
		}
	}
}

void CGrille::gameLoopProie()
{
	deplacement();
	for (auto& ligne : grille)
		for (auto& cellule : ligne)
			retirerMorts(cellule);
	reproductionProies();
}

// si les prédateurs se battent, un seul survit par cellule
void CGrille::gameLoopPredateur()
{
	deplacement();
	for (auto& ligne : grille)
	{
		for (auto& cellule : ligne)
		{
			retirerMorts(cellule);
			if (!seBatteC || cellule.predateurs.size() <= 1)
				continue;

			ajouterAnimation(CombatPredateur, cellule.predateurs[0]->getPosition());
			CPredateur* gagnant = cellule.predateurs[randomizer(0, static_cast<int>(cellule.predateurs.size()) - 1)];
			for (auto predateur : cellule.predateurs)
				if (predateur != gagnant)
					delete predateur;
			cellule.predateurs = { gagnant };
		}
	}
}

// avec combat : les mâles se battent, le gagnant peut se reproduire avec une femelle
// sans combat : chaque mâle est associé à une femelle et le couple peut se reproduire
void CGrille::combatEtReproductionPredateurs(CCellule& cellule, int x, int y)
{
	std::vector<CPredateur*> males;
	std::vector<CPredateur*> femelles;
	for (auto predateur : cellule.predateurs)
		(predateur->getSexe() == Sexe::Male ? males : femelles).push_back(predateur);

	if (!seBatteC)
	{
		size_t nbCouples = std::min(males.size(), femelles.size());
		for (size_t i = 0; i < nbCouples; ++i)
			naissancePredateur(cellule, males[i], femelles[i], x, y);
		return;
	}

	if (males.empty())
		return;

	CPredateur* male = males[randomizer(0, static_cast<int>(males.size()) - 1)];
	if (males.size() > 1)
	{
		ajouterAnimation(CombatPredateur, male->getPosition());
		for (auto perdant : males)
			if (perdant != male)
				delete perdant;
	}

	cellule.predateurs = femelles;
	cellule.predateurs.push_back(male);

	if (!femelles.empty())
		naissancePredateur(cellule, male, femelles[randomizer(0, static_cast<int>(femelles.size()) - 1)], x, y);
}

// le couple a une probabilité tauxReproductionC de donner un louveteau
void CGrille::naissancePredateur(CCellule& cellule, CPredateur* male, CPredateur* femelle, int x, int y)
{
	if (!male->peutSeReproduire() || !femelle->peutSeReproduire() || randomReel() >= tauxReproductionC)
		return;

	cellule.predateurs.push_back(new CPredateur(x, y));
	ajouterAnimation(NaissancePredateur, male->getPosition());

	if (estCeQueTempsAvantReproductionC)
	{
		male->setTempsAvantReproduction(tempsAvantReproductionC);
		femelle->setTempsAvantReproduction(tempsAvantReproductionC);
	}
}

// chaque prédateur affamé mange une proie de sa cellule
void CGrille::predation(CCellule& cellule)
{
	for (auto predateur : cellule.predateurs)
	{
		if (cellule.proies.empty())
			return;

		if (predateur->getSatiete() < tempsMortDeFaimC / 2)
		{
			int index = randomizer(0, static_cast<int>(cellule.proies.size()) - 1);
			ajouterAnimation(ProieMangee, cellule.proies[index]->getPosition());
			delete cellule.proies[index];
			cellule.proies.erase(cellule.proies.begin() + index);
			predateur->setSatiete(tempsMortDeFaimC);
		}
	}
}

void CGrille::gameLoopProiePredateur()
{
	deplacement();
	for (size_t y = 0; y < grille.size(); ++y)
	{
		for (size_t x = 0; x < grille[y].size(); ++x)
		{
			auto& cellule = grille[y][x];
			retirerMorts(cellule);
			combatEtReproductionPredateurs(cellule, static_cast<int>(x), static_cast<int>(y));
			predation(cellule);
		}
	}
	reproductionProies();
}
