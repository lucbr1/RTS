#include "UI.hpp"
#include "Variable.hpp"
#include <cmath>
#include <algorithm>

// ---------------------------------------------------------------------------
// Utilitaires
// ---------------------------------------------------------------------------

sf::String utf8(const std::string& s)
{
	return sf::String::fromUtf8(s.begin(), s.end());
}

sf::ConvexShape rectangleArrondi(sf::Vector2f taille, float rayon, sf::Color couleur)
{
	const unsigned int pointsParCoin = 8;
	const float pi = 3.14159265f;
	rayon = std::min(rayon, std::min(taille.x, taille.y) / 2.f);

	// centres des 4 coins, dans l'ordre : haut-gauche, haut-droit, bas-droit, bas-gauche
	const sf::Vector2f centres[4] = {
		{ rayon, rayon },
		{ taille.x - rayon, rayon },
		{ taille.x - rayon, taille.y - rayon },
		{ rayon, taille.y - rayon }
	};

	sf::ConvexShape forme(pointsParCoin * 4);
	for (unsigned int coin = 0; coin < 4; ++coin)
	{
		float angleDepart = pi + coin * pi / 2.f;
		for (unsigned int i = 0; i < pointsParCoin; ++i)
		{
			float angle = angleDepart + (pi / 2.f) * i / (pointsParCoin - 1);
			forme.setPoint(coin * pointsParCoin + i, centres[coin] + sf::Vector2f(std::cos(angle) * rayon, std::sin(angle) * rayon));
		}
	}
	forme.setFillColor(couleur);
	return forme;
}

// le baseline de SFML est à characterSize sous la position, d'où l'origine à 0.64 * taille
sf::Text creerTexte(const sf::Font& font, const sf::String& s, unsigned int taille, sf::Color couleur, sf::Vector2f position, bool gras)
{
	sf::Text texte(font, s, taille);
	texte.setFillColor(couleur);
	if (gras)
		texte.setStyle(sf::Text::Bold);
	texte.setOrigin({ 0.f, taille * 0.64f });
	texte.setPosition({ std::round(position.x), std::round(position.y) });
	return texte;
}

static sf::Color melanger(sf::Color a, sf::Color b, float t)
{
	auto m = [t](std::uint8_t x, std::uint8_t y) { return static_cast<std::uint8_t>(x + (y - x) * t); };
	return sf::Color(m(a.r, b.r), m(a.g, b.g), m(a.b, b.b), m(a.a, b.a));
}

// couleur atténuée pour un élément désactivé
static sf::Color estompe(sf::Color c, bool actif)
{
	return actif ? c : melanger(c, Theme::carte, 0.65f);
}

// fond d'une fenêtre (menu, graphique) avec son ombre
static void dessinerPanneau(sf::RenderTarget& cible, sf::FloatRect zone)
{
	sf::ConvexShape ombre = rectangleArrondi(zone.size, 16.f, sf::Color(0, 0, 0, 90));
	ombre.setPosition(zone.position + sf::Vector2f(0.f, 8.f));
	cible.draw(ombre);

	sf::ConvexShape fond = rectangleArrondi(zone.size, 16.f, Theme::panneau);
	fond.setPosition(zone.position);
	cible.draw(fond);
}

static void dessinerCroix(sf::RenderTarget& cible, sf::FloatRect bouton, sf::Vector2f souris)
{
	bool survol = bouton.contains(souris);
	if (survol)
	{
		sf::ConvexShape fond = rectangleArrondi(bouton.size, 8.f, Theme::survol);
		fond.setPosition(bouton.position);
		cible.draw(fond);
	}
	sf::Vector2f centre = bouton.position + bouton.size / 2.f;
	for (float angle : { 45.f, -45.f })
	{
		sf::RectangleShape trait({ 16.f, 2.f });
		trait.setOrigin({ 8.f, 1.f });
		trait.setPosition(centre);
		trait.setRotation(sf::degrees(angle));
		trait.setFillColor(survol ? Theme::texte : Theme::texteDiscret);
		cible.draw(trait);
	}
}

// ---------------------------------------------------------------------------
// CWidget
// ---------------------------------------------------------------------------

CWidget::CWidget(const sf::Font& font, const std::string& libelle, sf::Color accent, const bool* actif)
	: font(&font), libelle(utf8(libelle)), accent(accent), actif(actif)
{
}

// ---------------------------------------------------------------------------
// CInterrupteur
// ---------------------------------------------------------------------------

CInterrupteur::CInterrupteur(const sf::Font& font, const std::string& libelle, bool& valeur, sf::Color accent, bool resetRequis)
	: CWidget(font, libelle, accent), valeur(&valeur), curseur(valeur ? 1.f : 0.f)
{
	this->resetRequis = resetRequis;
}

bool CInterrupteur::appuyer(sf::Vector2f souris)
{
	if (!zone.contains(souris))
		return false;
	*valeur = !*valeur;
	return true;
}

void CInterrupteur::animer(float dt)
{
	float cible = *valeur ? 1.f : 0.f;
	float pas = dt * 8.f;
	if (std::abs(cible - curseur) <= pas)
		curseur = cible;
	else
		curseur += (cible > curseur) ? pas : -pas;
}

void CInterrupteur::dessiner(sf::RenderTarget& cible, sf::Vector2f souris) const
{
	const float centreY = zone.position.y + zone.size.y / 2.f;

	// fond de la ligne au survol
	if (zone.contains(souris))
	{
		sf::ConvexShape fond = rectangleArrondi(zone.size, 8.f, Theme::survol);
		fond.setPosition(zone.position);
		cible.draw(fond);
	}

	// libellé
	cible.draw(creerTexte(*font, libelle, 17, Theme::texte, { zone.position.x + 12.f, centreY }));

	// interrupteur
	const sf::Vector2f tailleInter(44.f, 24.f);
	const sf::Vector2f posInter(zone.position.x + zone.size.x - tailleInter.x - 12.f, centreY - tailleInter.y / 2.f);

	sf::ConvexShape pisteInter = rectangleArrondi(tailleInter, tailleInter.y / 2.f, melanger(Theme::interrupteurOff, accent, curseur));
	pisteInter.setPosition(posInter);
	cible.draw(pisteInter);

	const float rayon = 9.f;
	const float marge = (tailleInter.y - 2.f * rayon) / 2.f;
	sf::CircleShape bouton(rayon, 24);
	bouton.setOrigin({ rayon, rayon });
	bouton.setFillColor(sf::Color::White);
	float xMin = posInter.x + marge + rayon;
	float xMax = posInter.x + tailleInter.x - marge - rayon;
	bouton.setPosition({ xMin + (xMax - xMin) * curseur, centreY });
	cible.draw(bouton);
}

// ---------------------------------------------------------------------------
// CCurseur
// ---------------------------------------------------------------------------

CCurseur::CCurseur(const sf::Font& font, const std::string& libelle, unsigned int& valeur, unsigned int min, unsigned int max,
	sf::Color accent, const bool* actif, const std::string& unite, unsigned int pas, bool resetRequis)
	: CWidget(font, libelle, accent, actif), entier(&valeur), min(min), max(max), pas(std::max(1u, pas)), unite(unite)
{
	this->resetRequis = resetRequis;
	valeur = std::clamp(valeur, min, max);
}

CCurseur::CCurseur(const sf::Font& font, const std::string& libelle, double& taux, sf::Color accent, const bool* actif)
	: CWidget(font, libelle, accent, actif), taux(&taux), min(0), max(100), pas(1), unite(" %")
{
	taux = std::clamp(taux, 0.0, 1.0);
}

unsigned int CCurseur::lire() const
{
	return entier ? *entier : static_cast<unsigned int>(std::round(*taux * 100.0));
}

void CCurseur::ecrire(unsigned int v)
{
	if (entier)
		*entier = v;
	else
		*taux = v / 100.0;
}

sf::FloatRect CCurseur::piste() const
{
	return sf::FloatRect({ zone.position.x + 14.f, zone.position.y + 44.f }, { zone.size.x - 28.f, 4.f });
}

bool CCurseur::majDepuisSouris(sf::Vector2f souris)
{
	sf::FloatRect p = piste();
	float t = std::clamp((souris.x - p.position.x) / p.size.x, 0.f, 1.f);
	unsigned int nbPas = static_cast<unsigned int>(std::round(t * (max - min) / pas));
	unsigned int nouvelle = std::min(max, min + nbPas * pas);
	if (nouvelle == lire())
		return false;
	ecrire(nouvelle);
	modifie = true;
	return true;
}

// pour un curseur qui relance la simulation, on attend le relâchement (renvoie false ici)
bool CCurseur::appuyer(sf::Vector2f souris)
{
	if (!estActif())
		return false;
	// zone cliquable : la moitié basse de la ligne (autour de la piste)
	sf::FloatRect p = piste();
	sf::FloatRect zoneClic({ zone.position.x, p.position.y - 14.f }, { zone.size.x, 32.f });
	if (!zoneClic.contains(souris))
		return false;
	enGlissement = true;
	modifie = false;
	return majDepuisSouris(souris) && !resetRequis;
}

bool CCurseur::glisser(sf::Vector2f souris)
{
	if (!enGlissement)
		return false;
	return majDepuisSouris(souris) && !resetRequis;
}

bool CCurseur::relacher()
{
	bool aChange = enGlissement && modifie;
	enGlissement = false;
	modifie = false;
	return aChange;
}

void CCurseur::dessiner(sf::RenderTarget& cible, sf::Vector2f souris) const
{
	const bool actifMaintenant = estActif();
	const sf::FloatRect p = piste();

	if (actifMaintenant && (zone.contains(souris) || enGlissement))
	{
		sf::ConvexShape fond = rectangleArrondi(zone.size, 8.f, Theme::survol);
		fond.setPosition(zone.position);
		cible.draw(fond);
	}

	// libellé et valeur
	const float yTexte = zone.position.y + 20.f;
	cible.draw(creerTexte(*font, libelle, 15, estompe(Theme::texteDiscret, actifMaintenant), { zone.position.x + 12.f, yTexte }));

	sf::Text texteValeur = creerTexte(*font, utf8(std::to_string(lire()) + unite), 16, estompe(accent, actifMaintenant), { 0.f, yTexte }, true);
	texteValeur.setPosition({ std::round(zone.position.x + zone.size.x - 12.f - texteValeur.getLocalBounds().size.x), std::round(yTexte) });
	cible.draw(texteValeur);

	// piste
	float t = (max > min) ? static_cast<float>(lire() - min) / (max - min) : 0.f;

	sf::ConvexShape fondPiste = rectangleArrondi(p.size, 2.f, estompe(Theme::interrupteurOff, actifMaintenant));
	fondPiste.setPosition(p.position);
	cible.draw(fondPiste);

	if (t > 0.f)
	{
		sf::ConvexShape remplie = rectangleArrondi({ p.size.x * t, p.size.y }, 2.f, estompe(accent, actifMaintenant));
		remplie.setPosition(p.position);
		cible.draw(remplie);
	}

	// poignée
	const float rayon = enGlissement ? 9.f : 8.f;
	sf::CircleShape poignee(rayon, 24);
	poignee.setOrigin({ rayon, rayon });
	poignee.setPosition({ p.position.x + p.size.x * t, p.position.y + p.size.y / 2.f });
	poignee.setFillColor(estompe(sf::Color::White, actifMaintenant));
	cible.draw(poignee);
}

// ---------------------------------------------------------------------------
// CInterface
// ---------------------------------------------------------------------------

namespace
{
	const float largeurPanneau = 700.f;
	const float padding = 28.f;
	const float gouttiere = 20.f;
	const float enteteColonne = 52.f;
	const float basColonne = 8.f;
}

CInterface::CInterface(const sf::Font& font)
	: font(font),
	courbeProies(Theme::proie, "Jours", "Population"),
	courbePredateurs(Theme::predateur, "Jours", "Population"),
	phases(Theme::general, "Proies", "Prédateurs")
{
	// --- général
	general.push_back(std::make_unique<CInterrupteur>(font, "Animations", playAnimations, Theme::general));
	general.push_back(std::make_unique<CInterrupteur>(font, "Grille", afficherGrille, Theme::general));

	// --- proies
	const sf::Color p = Theme::proie;
	proies.push_back(std::make_unique<CInterrupteur>(font, "Apparition", spawnH, p, true));
	proies.push_back(std::make_unique<CCurseur>(font, "Population initiale", popInitialeH, 0, 1000, p, &spawnH, "", 10, true));
	proies.push_back(std::make_unique<CInterrupteur>(font, "Meurent de faim", meurtDeFaimH, p));
	proies.push_back(std::make_unique<CCurseur>(font, "Jours sans manger", tempsMortDeFaimH, 1, 50, p, &meurtDeFaimH));
	proies.push_back(std::make_unique<CInterrupteur>(font, "Délai de reproduction", estCeQueTempsAvantReproductionH, p));
	proies.push_back(std::make_unique<CCurseur>(font, "Jours entre reproductions", tempsAvantReproductionH, 0, 50, p, &estCeQueTempsAvantReproductionH));
	proies.push_back(std::make_unique<CCurseur>(font, "Taux de reproduction", tauxReproductionH, p));
	proies.push_back(std::make_unique<CInterrupteur>(font, "Mort naturelle", mortNaturelleH, p));
	proies.push_back(std::make_unique<CCurseur>(font, "Taux de mortalité", tauxMortaliteH, p, &mortNaturelleH));

	// --- prédateurs
	const sf::Color c = Theme::predateur;
	predateurs.push_back(std::make_unique<CInterrupteur>(font, "Apparition", spawnC, c, true));
	predateurs.push_back(std::make_unique<CCurseur>(font, "Population initiale", popInitialeC, 0, 1000, c, &spawnC, "", 10, true));
	predateurs.push_back(std::make_unique<CInterrupteur>(font, "Meurent de faim", meurtDeFaimC, c));
	predateurs.push_back(std::make_unique<CCurseur>(font, "Jours sans manger", tempsMortDeFaimC, 1, 50, c, &meurtDeFaimC));
	predateurs.push_back(std::make_unique<CInterrupteur>(font, "Délai de reproduction", estCeQueTempsAvantReproductionC, c));
	predateurs.push_back(std::make_unique<CCurseur>(font, "Jours entre reproductions", tempsAvantReproductionC, 0, 50, c, &estCeQueTempsAvantReproductionC));
	predateurs.push_back(std::make_unique<CCurseur>(font, "Taux de reproduction", tauxReproductionC, c));
	predateurs.push_back(std::make_unique<CInterrupteur>(font, "Mort naturelle", mortNaturelleC, c));
	predateurs.push_back(std::make_unique<CCurseur>(font, "Taux de mortalité", tauxMortaliteC, c, &mortNaturelleC));
	predateurs.push_back(std::make_unique<CInterrupteur>(font, "Se battent entre eux", seBatteC, c));

	calculerDisposition();
}

static float hauteurTotale(const ListeWidgets& liste)
{
	float h = 0.f;
	for (const auto& w : liste)
		h += w->hauteur();
	return h;
}

void CInterface::calculerDisposition()
{
	// bouton du menu dans la barre latérale
	boutonMenu = sf::FloatRect({ windowWidth + 12.f, 16.f }, { placeBoutton - 24.f, 44.f });

	// boutons des graphiques sous les statistiques
	for (int i = 0; i < 4; ++i)
		boutonsGraphiques[i] = sf::FloatRect({ windowWidth + 12.f, 384.f + i * 48.f }, { placeBoutton - 24.f, 40.f });

	// fenêtre des graphiques, centrée sur la zone de simulation
	const sf::Vector2f tailleGraphique(std::min(1100.f, windowWidth - 80.f), std::min(640.f, windowHeight - 80.f));
	panneauGraphique = sf::FloatRect({ std::round((windowWidth - tailleGraphique.x) / 2.f), std::round((windowHeight - tailleGraphique.y) / 2.f) }, tailleGraphique);
	boutonFermerGraphique = sf::FloatRect({ panneauGraphique.position.x + tailleGraphique.x - padding - 32.f, panneauGraphique.position.y + 22.f }, { 32.f, 32.f });

	const float largeurUtile = largeurPanneau - 2.f * padding;
	const float largeurColonne = (largeurUtile - gouttiere) / 2.f;

	// section générale : les éléments sont répartis sur 2 colonnes
	float hauteurGeneral = 0.f;
	for (size_t i = 0; i < general.size(); i += 2)
		hauteurGeneral += general[i]->hauteur();

	const float hauteurColonne = enteteColonne + std::max(hauteurTotale(proies), hauteurTotale(predateurs)) + basColonne;
	const float hauteur = 106.f + hauteurGeneral + 24.f + hauteurColonne + 52.f;

	const sf::Vector2f tailleFenetre(static_cast<float>(windowWidth + placeBoutton), static_cast<float>(windowHeight));
	panneau = sf::FloatRect({ std::round((tailleFenetre.x - largeurPanneau) / 2.f), std::round((tailleFenetre.y - hauteur) / 2.f) },
		{ largeurPanneau, hauteur });

	const float x = panneau.position.x + padding;
	const float y = panneau.position.y;

	boutonFermer = sf::FloatRect({ panneau.position.x + largeurPanneau - padding - 32.f, y + 22.f }, { 32.f, 32.f });

	// général
	float yGeneral = y + 106.f;
	for (size_t i = 0; i < general.size(); ++i)
	{
		float xCol = x + (i % 2) * (largeurColonne + gouttiere);
		general[i]->placer({ xCol, yGeneral }, largeurColonne);
		if (i % 2 == 1 || i + 1 == general.size())
			yGeneral += general[i]->hauteur();
	}

	// colonnes
	ySeparateur = yGeneral + 12.f;
	const float yColonnes = yGeneral + 24.f;
	zoneProies = sf::FloatRect({ x, yColonnes }, { largeurColonne, hauteurColonne });
	zonePredateurs = sf::FloatRect({ x + largeurColonne + gouttiere, yColonnes }, { largeurColonne, hauteurColonne });

	auto placerColonne = [](ListeWidgets& liste, sf::FloatRect zoneCol)
	{
		float yLigne = zoneCol.position.y + enteteColonne;
		for (auto& w : liste)
		{
			w->placer({ zoneCol.position.x + 6.f, yLigne }, zoneCol.size.x - 12.f);
			yLigne += w->hauteur();
		}
	};
	placerColonne(proies, zoneProies);
	placerColonne(predateurs, zonePredateurs);
}

bool CInterface::gererEvenement(const sf::Event& event, const sf::RenderWindow& window)
{
	if (const auto* touche = event.getIf<sf::Event::KeyPressed>())
	{
		if (touche->code == sf::Keyboard::Key::Escape)
		{
			if (ouvert)
				ouvert = false;
			else
				vue = Vue::Aucune;
		}
		else if (touche->code == sf::Keyboard::Key::M)
			basculer();
		return false;
	}

	bool reset = false;

	// glissement des curseurs
	if (const auto* deplacement = event.getIf<sf::Event::MouseMoved>())
	{
		if (!ouvert)
			return false;
		sf::Vector2f souris = window.mapPixelToCoords(deplacement->position);
		for (auto* groupe : { &general, &proies, &predateurs })
			for (auto& w : *groupe)
				if (w->glisser(souris) && w->resetRequis)
					reset = true;
		return reset;
	}

	if (const auto* relache = event.getIf<sf::Event::MouseButtonReleased>())
	{
		if (relache->button == sf::Mouse::Button::Left)
			for (auto* groupe : { &general, &proies, &predateurs })
				for (auto& w : *groupe)
					if (w->relacher() && w->resetRequis)
						reset = true;
		return reset;
	}

	const auto* clicSouris = event.getIf<sf::Event::MouseButtonPressed>();
	if (!clicSouris || clicSouris->button != sf::Mouse::Button::Left)
		return false;

	sf::Vector2f souris = window.mapPixelToCoords(clicSouris->position);

	if (boutonMenu.contains(souris))
	{
		basculer();
		return false;
	}

	for (int i = 0; i < 4; ++i)
	{
		if (boutonsGraphiques[i].contains(souris))
		{
			Vue choisie = static_cast<Vue>(i + 1);
			vue = (vue == choisie) ? Vue::Aucune : choisie;
			ouvert = false;
			return false;
		}
	}

	if (!ouvert)
	{
		// clic sur la croix ou sur la simulation en dehors du graphique
		if (boutonFermerGraphique.contains(souris) || (!panneauGraphique.contains(souris) && souris.x < windowWidth))
			vue = Vue::Aucune;
		return false;
	}

	if (boutonFermer.contains(souris) || !panneau.contains(souris))
	{
		ouvert = false; // clic sur la croix ou en dehors du panneau
		return false;
	}

	for (auto* groupe : { &general, &proies, &predateurs })
		for (auto& w : *groupe)
			if (w->appuyer(souris) && w->resetRequis)
				reset = true;
	return reset;
}

void CInterface::animer(float dt)
{
	for (auto* groupe : { &general, &proies, &predateurs })
		for (auto& w : *groupe)
			w->animer(dt);
}

void CInterface::dessiner(sf::RenderTarget& cible, sf::Vector2f souris, unsigned int nbProies, unsigned int nbPredateurs, unsigned int jour) const
{
	dessinerBarreLaterale(cible, souris, nbProies, nbPredateurs, jour);
	if (vue != Vue::Aucune)
		dessinerGraphique(cible, souris);
	if (ouvert)
		dessinerMenu(cible, souris);
}

void CInterface::basculer()
{
	ouvert = !ouvert;
	if (ouvert)
		vue = Vue::Aucune;
}

void CInterface::enregistrer(unsigned int jour, unsigned int nbProies, unsigned int nbPredateurs)
{
	courbeProies.ajouterPoint(jour, nbProies);
	courbePredateurs.ajouterPoint(jour, nbPredateurs);
	phases.ajouterPoint(nbProies, nbPredateurs);
}

void CInterface::effacerGraphiques()
{
	courbeProies.effacer();
	courbePredateurs.effacer();
	phases.effacer();
}

void CInterface::dessinerBarreLaterale(sf::RenderTarget& cible, sf::Vector2f souris, unsigned int nbProies, unsigned int nbPredateurs, unsigned int jour) const
{
	const float x = static_cast<float>(windowWidth);

	sf::RectangleShape fond({ static_cast<float>(placeBoutton), static_cast<float>(windowHeight) });
	fond.setPosition({ x, 0.f });
	fond.setFillColor(Theme::fond);
	cible.draw(fond);

	// bouton menu (icône hamburger + texte)
	bool survole = boutonMenu.contains(souris) || ouvert;
	sf::ConvexShape bouton = rectangleArrondi(boutonMenu.size, 10.f, survole ? Theme::survol : Theme::carte);
	bouton.setPosition(boutonMenu.position);
	cible.draw(bouton);

	const float centreY = boutonMenu.position.y + boutonMenu.size.y / 2.f;
	for (int i = -1; i <= 1; ++i)
	{
		sf::RectangleShape barre({ 18.f, 2.f });
		barre.setPosition({ boutonMenu.position.x + 14.f, centreY - 1.f + i * 6.f });
		barre.setFillColor(Theme::texte);
		cible.draw(barre);
	}
	cible.draw(creerTexte(font, utf8("Menu"), 18, Theme::texte, { boutonMenu.position.x + 44.f, centreY }, true));

	// statistiques
	struct Stat { const char* nom; unsigned int valeur; sf::Color couleur; };
	const Stat stats[3] = {
		{ "Jour", jour, Theme::texte },
		{ "Proies", nbProies, Theme::proie },
		{ "Prédateurs", nbPredateurs, Theme::predateur }
	};

	float y = 96.f;
	for (const auto& stat : stats)
	{
		sf::ConvexShape carte = rectangleArrondi({ placeBoutton - 24.f, 70.f }, 10.f, Theme::carte);
		carte.setPosition({ x + 12.f, y });
		cible.draw(carte);

		sf::RectangleShape liseret({ 3.f, 46.f });
		liseret.setPosition({ x + 12.f, y + 12.f });
		liseret.setFillColor(stat.couleur);
		cible.draw(liseret);

		cible.draw(creerTexte(font, utf8(stat.nom), 13, Theme::texteDiscret, { x + 26.f, y + 22.f }));
		cible.draw(creerTexte(font, std::to_string(stat.valeur), 26, stat.couleur, { x + 26.f, y + 48.f }, true));
		y += 82.f;
	}

	// boutons des graphiques
	struct BoutonGraphique { const char* nom; sf::Color couleur; Vue vue; };
	const BoutonGraphique boutons[4] = {
		{ "Proies", Theme::proie, Vue::Proies },
		{ "Prédateurs", Theme::predateur, Vue::Predateurs },
		{ "Les deux", Theme::proie, Vue::LesDeux },
		{ "Phases", Theme::general, Vue::Phases }
	};

	cible.draw(creerTexte(font, utf8("GRAPHIQUES"), 12, Theme::texteDiscret, { x + 14.f, boutonsGraphiques[0].position.y - 18.f }, true));
	for (int i = 0; i < 4; ++i)
	{
		const sf::FloatRect& zone = boutonsGraphiques[i];
		const bool actif = vue == boutons[i].vue;

		sf::ConvexShape fondBouton = rectangleArrondi(zone.size, 10.f, (actif || zone.contains(souris)) ? Theme::survol : Theme::carte);
		fondBouton.setPosition(zone.position);
		cible.draw(fondBouton);

		if (actif)
		{
			sf::RectangleShape liseret({ 3.f, zone.size.y - 16.f });
			liseret.setPosition({ zone.position.x, zone.position.y + 8.f });
			liseret.setFillColor(boutons[i].couleur);
			cible.draw(liseret);
		}

		const float centreBouton = zone.position.y + zone.size.y / 2.f;
		sf::CircleShape pastille(5.f, 16);
		pastille.setOrigin({ 5.f, 5.f });
		pastille.setPosition({ zone.position.x + 18.f, centreBouton });
		pastille.setFillColor(boutons[i].couleur);
		cible.draw(pastille);
		if (boutons[i].vue == Vue::LesDeux)
		{
			// deuxième pastille (prédateurs) qui chevauche la première
			pastille.move({ 6.f, 0.f });
			pastille.setFillColor(Theme::predateur);
			cible.draw(pastille);
		}

		cible.draw(creerTexte(font, utf8(boutons[i].nom), 15, Theme::texte, { zone.position.x + 36.f, centreBouton }, actif));
	}

	// aide en bas
	cible.draw(creerTexte(font, utf8("Espace : jour suivant"), 12, Theme::texteDiscret, { x + 14.f, windowHeight - 44.f }));
	cible.draw(creerTexte(font, utf8("M : menu"), 12, Theme::texteDiscret, { x + 14.f, windowHeight - 24.f }));
}

void CInterface::dessinerColonne(sf::RenderTarget& cible, sf::Vector2f souris, const ListeWidgets& lignes, const std::string& titre, sf::Color accent, sf::FloatRect zone) const
{
	sf::ConvexShape carte = rectangleArrondi(zone.size, 12.f, Theme::carte);
	carte.setPosition(zone.position);
	cible.draw(carte);

	// pastille de couleur + titre
	sf::CircleShape pastille(5.f, 16);
	pastille.setOrigin({ 5.f, 5.f });
	pastille.setPosition({ zone.position.x + 22.f, zone.position.y + 28.f });
	pastille.setFillColor(accent);
	cible.draw(pastille);
	cible.draw(creerTexte(font, utf8(titre), 20, Theme::texte, { zone.position.x + 36.f, zone.position.y + 28.f }, true));

	for (const auto& w : lignes)
		w->dessiner(cible, souris);
}

void CInterface::dessinerMenu(sf::RenderTarget& cible, sf::Vector2f souris) const
{
	// voile sombre sur toute la fenêtre
	sf::RectangleShape voile({ static_cast<float>(windowWidth + placeBoutton), static_cast<float>(windowHeight) });
	voile.setFillColor(sf::Color(0, 0, 0, 150));
	cible.draw(voile);

	dessinerPanneau(cible, panneau);

	const float x = panneau.position.x + padding;
	const float y = panneau.position.y;

	// titre
	cible.draw(creerTexte(font, utf8("Paramètres"), 28, Theme::texte, { x, y + 38.f }, true));

	dessinerCroix(cible, boutonFermer, souris);

	// section générale
	cible.draw(creerTexte(font, utf8("GÉNÉRAL"), 13, Theme::texteDiscret, { x, y + 92.f }, true));
	for (const auto& w : general)
		w->dessiner(cible, souris);

	// séparateur
	sf::RectangleShape separateur({ panneau.size.x - 2.f * padding, 1.f });
	separateur.setPosition({ x, ySeparateur });
	separateur.setFillColor(Theme::carte);
	cible.draw(separateur);

	// colonnes proies / prédateurs
	dessinerColonne(cible, souris, proies, "Proies", Theme::proie, zoneProies);
	dessinerColonne(cible, souris, predateurs, "Prédateurs", Theme::predateur, zonePredateurs);

	// pied de page
	cible.draw(creerTexte(font, utf8("« Apparition » relance la simulation   ·   Échap pour fermer"), 13, Theme::texteDiscret,
		{ x, panneau.position.y + panneau.size.y - 26.f }));
}

void CInterface::dessinerGraphique(sf::RenderTarget& cible, sf::Vector2f souris) const
{
	// voile sombre sur la simulation seulement (la barre latérale reste visible)
	sf::RectangleShape voile({ static_cast<float>(windowWidth), static_cast<float>(windowHeight) });
	voile.setFillColor(sf::Color(0, 0, 0, 150));
	cible.draw(voile);

	dessinerPanneau(cible, panneauGraphique);
	dessinerCroix(cible, boutonFermerGraphique, souris);

	const CGraphique* graphique = &courbeProies;
	std::string titre = "Proies";
	std::string sousTitre = "Population de proies en fonction du temps";
	sf::Color accent = Theme::proie;
	if (vue == Vue::Predateurs)
	{
		graphique = &courbePredateurs;
		titre = "Prédateurs";
		sousTitre = "Population de prédateurs en fonction du temps";
		accent = Theme::predateur;
	}
	else if (vue == Vue::LesDeux)
	{
		titre = "Proies et prédateurs";
		sousTitre = "Populations en fonction du temps   ·   vert : proies   ·   rouge : prédateurs";
	}
	else if (vue == Vue::Phases)
	{
		graphique = &phases;
		titre = "Diagramme de phases";
		sousTitre = "Prédateurs en fonction des proies   ·   ○ départ   ● aujourd'hui";
		accent = Theme::general;
	}

	const float x = panneauGraphique.position.x + padding;
	const float y = panneauGraphique.position.y;

	sf::CircleShape pastille(6.f, 16);
	pastille.setOrigin({ 6.f, 6.f });
	pastille.setPosition({ x + 6.f, y + 38.f });
	pastille.setFillColor(accent);
	cible.draw(pastille);
	float xTitre = x + 22.f;
	if (vue == Vue::LesDeux)
	{
		pastille.move({ 8.f, 0.f });
		pastille.setFillColor(Theme::predateur);
		cible.draw(pastille);
		xTitre += 8.f;
	}
	cible.draw(creerTexte(font, utf8(titre), 28, Theme::texte, { xTitre, y + 38.f }, true));
	cible.draw(creerTexte(font, utf8(sousTitre), 14, Theme::texteDiscret, { x, y + 74.f }));

	const sf::FloatRect zone({ x, y + 100.f }, { panneauGraphique.size.x - 2.f * padding, panneauGraphique.size.y - 100.f - padding });
	const CGraphique* superposee = (vue == Vue::LesDeux) ? &courbePredateurs : nullptr;
	graphique->dessiner(cible, font, zone, vue == Vue::Phases, superposee);
}
