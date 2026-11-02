#include "Grille.hpp"
#include "Variable.hpp"
#include "fonctions.hpp"
#include <algorithm>

namespace {
	constexpr uint32_t ageReproduction = 30;
	constexpr uint32_t aucun = 0xFFFFFFFFu;
	constexpr int64_t tailleBloc = 1 << 16; // entités traitées par un même générateur aléatoire
}

CGrille::CGrille(sf::RenderWindow& window)
	: window(window),
	formeProie(cellSize / 2.0f),
	formePredateur(cellSize / 4.0f),
	etape(Etape::PROIE_SEUL),
	largeur(windowWidth / cellSize),
	hauteur(windowHeight / cellSize)
{
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

	formeProie.setFillColor(sf::Color::Green);
	formePredateur.setFillColor(sf::Color::Red);

	presenceProies.resize(nbCellules());
	presencePredateurs.resize(nbCellules());

	voisins.resize(nbCellules() * 9);
	for (uint32_t y = 0; y < hauteur; ++y)
		for (uint32_t x = 0; x < largeur; ++x)
		{
			size_t k = (y * largeur + x) * 9;
			for (int dy = -1; dy <= 1; ++dy)
				for (int dx = -1; dx <= 1; ++dx)
				{
					const uint32_t nx = (x + dx + largeur) % largeur;
					const uint32_t ny = (y + dy + hauteur) % hauteur;
					voisins[k++] = static_cast<uint16_t>(ny * largeur + nx);
				}
		}

	reset();
}

void CGrille::peuplerProies(size_t nb)
{
	for (size_t i = 0; i < nb; ++i)
	{
		proies.cell.push_back(static_cast<uint16_t>(randomBelow(nbCellules())));
		proies.delai.push_back(0); // déjà adulte
	}
}

void CGrille::peuplerPredateurs(size_t nb)
{
	for (size_t i = 0; i < nb; ++i)
		predateurs.ajouter(static_cast<uint16_t>(randomBelow(nbCellules())), ageReproduction, 10,
			random32() & 1 ? Sexe::Femelle : Sexe::Male);
}

void CGrille::reset()
{
	proies.vider();
	predateurs.vider();
	if (reservationProies > 0)
	{
		proies.cell.reserve(reservationProies);
		proies.delai.reserve(reservationProies);
	}

	if (etape == Etape::PREDATEUR_SEUL || etape == Etape::PROIE_PREDATEUR)
		peuplerPredateurs(popInitialeC);
	if (etape == Etape::PROIE_SEUL || etape == Etape::PROIE_PREDATEUR)
		peuplerProies(popInitialeH);

	presenceAJour = false;
}

void CGrille::majPresence()
{
	std::fill(presenceProies.begin(), presenceProies.end(), 0);
	std::fill(presencePredateurs.begin(), presencePredateurs.end(), 0);

	for (size_t i = 0, n = proies.size(); i < n; ++i)
		presenceProies[proies.cell[i]] = 1;
	for (size_t i = 0, n = predateurs.size(); i < n; ++i)
		presencePredateurs[predateurs.cell[i]] = 1;

	presenceAJour = true;
}

void CGrille::afficher()
{
	window.draw(vertexGrille);

	if (!presenceAJour)
		majPresence();
	else
	{
		// la passe des proies a déjà fourni leur présence, seuls les prédateurs restent à relever
		std::fill(presencePredateurs.begin(), presencePredateurs.end(), 0);
		for (size_t i = 0, n = predateurs.size(); i < n; ++i)
			presencePredateurs[predateurs.cell[i]] = 1;
	}

	const float taille = static_cast<float>(cellSize);
	for (uint32_t c = 0, n = nbCellules(); c < n; ++c)
	{
		const float px = (c % largeur) * taille;
		const float py = (c / largeur) * taille;

		if (presenceProies[c])
		{
			formeProie.setPosition({ px, py });
			window.draw(formeProie);
		}
		if (presencePredateurs[c])
		{
			formePredateur.setPosition({ px + taille / 4, py + taille / 4 });
			window.draw(formePredateur);
		}
	}
}

void CGrille::gameLoop()
{
	presenceAJour = false;

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

// Passe unique et parallèle sur toutes les proies : déplacement, compte à rebours de reproduction,
// et relevé par thread de la présence et des éligibles par cellule (fusionnés ensuite).
// Avec reproduire, les proies éligibles se mettent en attente par paires et les petits sont ajoutés.
void CGrille::deplacementProies(bool reproduire)
{
	const int64_t n = static_cast<int64_t>(proies.size());
	const uint32_t nbC = nbCellules();
	const int nbT = nbThreads();
	const int64_t nbBlocs = (n + tailleBloc - 1) / tailleBloc;
	const uint8_t attente = static_cast<uint8_t>(tempsAvantReproductionH);
	const uint64_t graine = (static_cast<uint64_t>(random32()) << 32) | random32();

	std::vector<std::vector<uint32_t>> eligibles(nbT, std::vector<uint32_t>(nbC, 0));
	std::vector<std::vector<int64_t>> derniers(nbT, std::vector<int64_t>(nbC, -1));
	std::vector<std::vector<uint8_t>> presents(nbT, std::vector<uint8_t>(nbC, 0));

	uint16_t* cell = proies.cell.data();
	uint8_t* delai = proies.delai.data();
	const uint16_t* vois = voisins.data();

	#pragma omp parallel
	{
		const int t = numThread();
		uint32_t* elig = eligibles[t].data();
		int64_t* der = derniers[t].data();
		uint8_t* pres = presents[t].data();

		#pragma omp for schedule(static)
		for (int64_t b = 0; b < nbBlocs; ++b)
		{
			Xorshift rng(graine, static_cast<uint64_t>(b));
			const int64_t fin = std::min(n, (b + 1) * tailleBloc);
			for (int64_t i = b * tailleBloc; i < fin; ++i)
			{
				const uint32_t c = vois[cell[i] * 9u + rng.sous(9)];
				cell[i] = static_cast<uint16_t>(c);
				pres[c] = 1;

				if (reproduire)
				{
					uint8_t d = delai[i];
					if (d > 0)
						d--;
					if (d == 0)
					{
						// éligible : mis en attente tout de suite, l'éventuel exclu est rétabli après
						elig[c]++;
						der[c] = i;
						d = attente;
					}
					delai[i] = d;
				}
			}
		}
	}

	std::fill(presenceProies.begin(), presenceProies.end(), 0);
	for (int t = 0; t < nbT; ++t)
		for (uint32_t c = 0; c < nbC; ++c)
			presenceProies[c] |= presents[t][c];
	presenceAJour = reproduire; // sans reproduction, des proies vont encore être mangées

	if (!reproduire)
		return;

	// Par cellule : paires de parents. Si le nombre d'éligibles est impair, le dernier rencontré reste libre.
	std::vector<uint64_t> debut(nbC + 1, 0);
	for (uint32_t c = 0; c < nbC; ++c)
	{
		uint32_t e = 0;
		int64_t dernier = -1;
		for (int t = 0; t < nbT; ++t)
		{
			e += eligibles[t][c];
			dernier = std::max(dernier, derniers[t][c]);
		}
		if (e & 1)
			delai[dernier] = 0;
		debut[c + 1] = debut[c] + e / 2;
	}

	// Les petits sont écrits cellule par cellule dans la zone ajoutée en fin de tableau
	const uint64_t nbNaissances = debut[nbC];
	proies.cell.resize(n + nbNaissances);
	proies.delai.resize(n + nbNaissances);
	cell = proies.cell.data();
	delai = proies.delai.data();

	#pragma omp parallel for schedule(dynamic)
	for (int64_t c = 0; c < static_cast<int64_t>(nbC); ++c)
	{
		std::fill(cell + n + debut[c], cell + n + debut[c + 1], static_cast<uint16_t>(c));
		std::fill(delai + n + debut[c], delai + n + debut[c + 1], static_cast<uint8_t>(ageReproduction));
	}
}

// Avec useSatiete, les prédateurs affamés meurent et sont retirés dans la même passe
void CGrille::deplacementPredateurs(bool useSatiete)
{
	const uint16_t* vois = voisins.data();
	SoaPredateurs& p = predateurs;

	size_t w = 0;
	for (size_t i = 0, n = p.size(); i < n; ++i)
	{
		uint8_t satiete = p.satiete[i];
		if (useSatiete)
		{
			if (satiete == 0)
				continue;
			satiete--;
		}

		p.cell[w] = vois[p.cell[i] * 9u + randomBelow(9)];
		p.attente[w] = p.attente[i] > 0 ? p.attente[i] - 1 : 0;
		p.age[w] = p.age[i] + 1;
		p.satiete[w] = satiete;
		p.sexe[w] = p.sexe[i];
		++w;
	}

	p.cell.resize(w);
	p.attente.resize(w);
	p.age.resize(w);
	p.satiete.resize(w);
	p.sexe.resize(w);
}

void CGrille::gameLoopProie()
{
	deplacementProies(true);
}

// Un seul prédateur, tiré au hasard, survit par cellule
void CGrille::gameLoopPredateur()
{
	deplacementPredateurs(false);

	const uint32_t nbC = nbCellules();
	const size_t n = predateurs.size();

	std::vector<uint32_t> compte(nbC, 0);
	for (size_t i = 0; i < n; ++i)
		compte[predateurs.cell[i]]++;

	std::vector<uint32_t> elu(nbC, 0);
	for (uint32_t c = 0; c < nbC; ++c)
		if (compte[c] > 0)
			elu[c] = randomBelow(compte[c]);

	std::vector<uint32_t> vus(nbC, 0);
	garder.resize(n);
	for (size_t i = 0; i < n; ++i)
	{
		const uint32_t c = predateurs.cell[i];
		garder[i] = vus[c]++ == elu[c];
	}

	predateurs.filtrer([&](size_t i) { return garder[i] != 0; });
}

void CGrille::gameLoopProiePredateur()
{
	deplacementPredateurs(true);
	deplacementProies(false);

	const uint32_t nbC = nbCellules();
	const size_t np = predateurs.size();

	// Par cellule : toutes les femelles restent, un seul mâle (tiré au hasard) survit
	std::vector<uint32_t> males(nbC, 0), femelles(nbC, 0);
	for (size_t i = 0; i < np; ++i)
		(predateurs.sexe[i] == Sexe::Male ? males : femelles)[predateurs.cell[i]]++;

	std::vector<uint32_t> eluMale(nbC, 0), eluFemelle(nbC, 0);
	for (uint32_t c = 0; c < nbC; ++c)
	{
		if (males[c] > 0) eluMale[c] = randomBelow(males[c]);
		if (femelles[c] > 0) eluFemelle[c] = randomBelow(femelles[c]);
	}

	std::vector<uint32_t> idxMale(nbC, aucun), idxFemelle(nbC, aucun);
	std::vector<uint32_t> vusM(nbC, 0), vusF(nbC, 0);
	garder.resize(np);
	for (size_t i = 0; i < np; ++i)
	{
		const uint32_t c = predateurs.cell[i];
		if (predateurs.sexe[i] == Sexe::Male)
		{
			const bool elu = vusM[c]++ == eluMale[c];
			if (elu) idxMale[c] = static_cast<uint32_t>(i);
			garder[i] = elu;
		}
		else
		{
			if (vusF[c]++ == eluFemelle[c]) idxFemelle[c] = static_cast<uint32_t>(i);
			garder[i] = 1;
		}
	}

	// Reproduction : le couple élu fait un petit s'il est éligible
	const uint16_t attente = static_cast<uint16_t>(tempsAvantReproductionC);
	std::vector<uint32_t> naissances;
	for (uint32_t c = 0; c < nbC; ++c)
	{
		const uint32_t m = idxMale[c], f = idxFemelle[c];
		if (m == aucun || f == aucun)
			continue;
		if (predateurs.age[m] >= ageReproduction && predateurs.attente[m] == 0 &&
			predateurs.age[f] >= ageReproduction && predateurs.attente[f] == 0)
		{
			predateurs.attente[m] = attente;
			predateurs.attente[f] = attente;
			naissances.push_back(c);
		}
	}

	predateurs.filtrer([&](size_t i) { return garder[i] != 0; });
	for (uint32_t c : naissances)
		predateurs.ajouter(static_cast<uint16_t>(c), 0, 10, random32() & 1 ? Sexe::Femelle : Sexe::Male);

	// Prédation : chaque prédateur affamé mange une proie de sa cellule s'il en reste
	std::vector<uint32_t> restantes(nbC, 0);
	for (size_t i = 0, n = proies.size(); i < n; ++i)
		restantes[proies.cell[i]]++;

	std::vector<uint32_t> mangees(nbC, 0);
	bool aManger = false;
	for (size_t i = 0, n = predateurs.size(); i < n; ++i)
	{
		if (predateurs.satiete[i] >= 5)
			continue;
		const uint32_t c = predateurs.cell[i];
		if (mangees[c] < restantes[c])
		{
			mangees[c]++;
			predateurs.satiete[i] = 10;
			aManger = true;
		}
	}

	// Tirage uniforme de mangees[c] proies à retirer parmi restantes[c] (selection sampling)
	if (aManger)
	{
		proies.filtrer([&](size_t i) {
			const uint32_t c = proies.cell[i];
			const uint32_t n = restantes[c]--;
			if (mangees[c] > 0 && randomBelow(n) < mangees[c])
			{
				mangees[c]--;
				return false;
			}
			return true;
		});
	}
}
