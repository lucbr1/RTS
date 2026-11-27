#include "UI.hpp"
#include "Variable.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

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

// interrupteur (piste arrondie + bouton rond), etat entre 0 (off) et 1 (on)
static void dessinerBascule(sf::RenderTarget& cible, sf::Vector2f position, sf::Vector2f taille, sf::Color accent, float etat)
{
	sf::ConvexShape piste = rectangleArrondi(taille, taille.y / 2.f, melanger(Theme::interrupteurOff, accent, etat));
	piste.setPosition(position);
	cible.draw(piste);

	const float rayon = taille.y / 2.f - 3.f;
	const float marge = (taille.y - 2.f * rayon) / 2.f;
	const float xMin = position.x + marge + rayon;
	const float xMax = position.x + taille.x - marge - rayon;
	sf::CircleShape bouton(rayon, 24);
	bouton.setOrigin({ rayon, rayon });
	bouton.setFillColor(sf::Color::White);
	bouton.setPosition({ xMin + (xMax - xMin) * etat, position.y + taille.y / 2.f });
	cible.draw(bouton);
}

// fait avancer une animation vers sa cible (0 ou 1)
static void avancerVers(float& valeur, float cible, float dt)
{
	float pas = dt * 8.f;
	if (std::abs(cible - valeur) <= pas)
		valeur = cible;
	else
		valeur += (cible > valeur) ? pas : -pas;
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
	avancerVers(curseur, *valeur ? 1.f : 0.f, dt);
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
	dessinerBascule(cible, { zone.position.x + zone.size.x - tailleInter.x - 12.f, centreY - tailleInter.y / 2.f }, tailleInter, accent, curseur);
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

CCurseur::CCurseur(const sf::Font& font, const std::string& libelle, double& valeur, unsigned int min, unsigned int max,
	sf::Color accent, const bool* actif, const std::string& unite)
	: CWidget(font, libelle, accent, actif), reel(&valeur), echelle(1.0), min(min), max(max), pas(1), unite(unite)
{
	valeur = std::clamp(std::round(valeur), static_cast<double>(min), static_cast<double>(max));
}

CCurseur::CCurseur(const sf::Font& font, const std::string& libelle, double& taux, sf::Color accent, const bool* actif)
	: CWidget(font, libelle, accent, actif), reel(&taux), echelle(100.0), min(0), max(100), pas(1), unite(" %")
{
	taux = std::clamp(taux, 0.0, 1.0);
}

CCurseur& CCurseur::avecInterrupteur(bool& valeur)
{
	interrupteur = &valeur;
	actif = &valeur;
	animInter = valeur ? 1.f : 0.f;
	return *this;
}

unsigned int CCurseur::lire() const
{
	if (resetRequis && enGlissement)
		return valeurGlissement;
	return entier ? *entier : static_cast<unsigned int>(std::round(*reel * echelle));
}

// pour un curseur qui relance la simulation, la vraie variable n'est modifiée qu'au relâchement :
// sinon la variable (ex : cellSize) change alors que la grille n'a pas encore été reconstruite
void CCurseur::ecrire(unsigned int v)
{
	if (resetRequis && enGlissement)
		valeurGlissement = v;
	else if (entier)
		*entier = v;
	else
		*reel = v / echelle;
}

sf::FloatRect CCurseur::piste() const
{
	return sf::FloatRect({ zone.position.x + 14.f, zone.position.y + 42.f }, { zone.size.x - 28.f, 4.f });
}

// moitié haute de la ligne (libellé + interrupteur)
sf::FloatRect CCurseur::zoneInterrupteur() const
{
	return sf::FloatRect(zone.position, { zone.size.x, 30.f });
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
	// clic sur la moitié haute : bascule l'interrupteur
	if (interrupteur && zoneInterrupteur().contains(souris))
	{
		*interrupteur = !*interrupteur;
		return true;
	}

	if (!estActif())
		return false;
	// zone cliquable : la moitié basse de la ligne (autour de la piste)
	sf::FloatRect zoneClic({ zone.position.x, zone.position.y + 30.f }, { zone.size.x, zone.size.y - 30.f });
	if (!zoneClic.contains(souris))
		return false;
	enGlissement = false;
	valeurGlissement = lire(); // valeur actuelle de la variable
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
	if (aChange && resetRequis)
		ecrire(valeurGlissement); // on applique la valeur maintenant (la grille va être réinitialisée)
	return aChange;
}

void CCurseur::animer(float dt)
{
	if (interrupteur)
		avancerVers(animInter, *interrupteur ? 1.f : 0.f, dt);
}

void CCurseur::dessiner(sf::RenderTarget& cible, sf::Vector2f souris) const
{
	const bool actifMaintenant = estActif();
	const sf::FloatRect p = piste();

	if (((actifMaintenant || interrupteur) && zone.contains(souris)) || enGlissement)
	{
		sf::ConvexShape fond = rectangleArrondi(zone.size, 8.f, Theme::survol);
		fond.setPosition(zone.position);
		cible.draw(fond);
	}

	// libellé, valeur et interrupteur sur la même ligne
	// (la place de l'interrupteur est toujours réservée pour que les valeurs soient alignées)
	const float yTexte = zone.position.y + 18.f;
	const sf::Vector2f tailleInter(36.f, 20.f);
	const float xInter = zone.position.x + zone.size.x - 12.f - tailleInter.x;

	cible.draw(creerTexte(*font, libelle, 16, actifMaintenant ? Theme::texte : Theme::texteDiscret, { zone.position.x + 12.f, yTexte }));

	sf::Text texteValeur = creerTexte(*font, utf8(std::to_string(lire()) + unite), 16, estompe(accent, actifMaintenant), { 0.f, yTexte }, true);
	texteValeur.setPosition({ std::round(xInter - 12.f - texteValeur.getLocalBounds().size.x), std::round(yTexte) });
	cible.draw(texteValeur);

	if (interrupteur)
		dessinerBascule(cible, { xInter, yTexte - tailleInter.y / 2.f }, tailleInter, accent, animInter);

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
	const float rayon = enGlissement ? 9.f : 7.f;
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
	const float largeurPanneau = 1090.f; // 3 colonnes : proies, prédateurs, herbe
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
	general.push_back(std::make_unique<CCurseur>(font, "Taille des cases", cellSize, 10, 50, Theme::general, nullptr, " px", 5, true));

	// chaque ligne : libellé + valeur, interrupteur éventuel à droite, curseur en dessous
	auto ajouter = [](ListeWidgets& liste, std::unique_ptr<CCurseur> curseur, bool* interrupteur = nullptr)
	{
		if (interrupteur)
			curseur->avecInterrupteur(*interrupteur);
		liste.push_back(std::move(curseur));
	};

	// --- proies
	const sf::Color p = Theme::proie;
	ajouter(proies, std::make_unique<CCurseur>(font, "Population initiale", popInitialeH, 0, 1000, p, nullptr, "", 10, true), &spawnH);
	ajouter(proies, std::make_unique<CCurseur>(font, "Taux de reproduction", tauxReproductionH, p));
	ajouter(proies, std::make_unique<CCurseur>(font, "Délai de reproduction", tempsAvantReproductionH, 0, 50, p), &estCeQueTempsAvantReproductionH);
	ajouter(proies, std::make_unique<CCurseur>(font, "Repro. si satiété >", seuilSatieteReproductionH, 0, 50, p), &reproductionEnFonctionDeLaSatieteH);
	ajouter(proies, std::make_unique<CCurseur>(font, "Mort de faim après", tempsMortDeFaimH, 1, 50, p), &meurtDeFaimH);
	ajouter(proies, std::make_unique<CCurseur>(font, "Mort naturelle", tauxMortaliteH, p), &mortNaturelleH);

	// --- prédateurs
	const sf::Color c = Theme::predateur;
	ajouter(predateurs, std::make_unique<CCurseur>(font, "Population initiale", popInitialeC, 0, 1000, c, nullptr, "", 10, true), &spawnC);
	ajouter(predateurs, std::make_unique<CCurseur>(font, "Taux de reproduction", tauxReproductionC, c));
	ajouter(predateurs, std::make_unique<CCurseur>(font, "Délai de reproduction", tempsAvantReproductionC, 0, 50, c), &estCeQueTempsAvantReproductionC);
	ajouter(predateurs, std::make_unique<CCurseur>(font, "Repro. si satiété >", seuilSatieteReproductionC, 0, 50, c), &reproductionEnFonctionDeLaSatieteC);
	ajouter(predateurs, std::make_unique<CCurseur>(font, "Mort de faim après", tempsMortDeFaimC, 1, 50, c), &meurtDeFaimC);
	ajouter(predateurs, std::make_unique<CCurseur>(font, "Mort naturelle", tauxMortaliteC, c), &mortNaturelleC);
	ajouter(predateurs, std::make_unique<CCurseur>(font, "Combats (mort)", probaCombat, c), &seBatteC);

	// --- herbe
	const sf::Color h = Theme::herbe;
	ajouter(herbe, std::make_unique<CCurseur>(font, "Quantité max par case", quantiteMaxHerbe, 0, 10, h, nullptr, "", 1, true));
	ajouter(herbe, std::make_unique<CCurseur>(font, "Temps de repousse", tempsAvantReapparitionHerbe, 1, 30, h));

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
	const float largeurColonne = (largeurUtile - 2.f * gouttiere) / 3.f;

	// section générale : les éléments sont répartis sur 3 colonnes
	float hauteurGeneral = 0.f;
	for (size_t i = 0; i < general.size(); i += 3)
	{
		float hauteurLigne = 0.f;
		for (size_t j = i; j < general.size() && j < i + 3; ++j)
			hauteurLigne = std::max(hauteurLigne, general[j]->hauteur());
		hauteurGeneral += hauteurLigne;
	}

	const float hauteurColonne = enteteColonne + std::max({ hauteurTotale(proies), hauteurTotale(predateurs), hauteurTotale(herbe) }) + basColonne;
	const float hauteur = 106.f + hauteurGeneral + 24.f + hauteurColonne + 52.f;

	const sf::Vector2f tailleFenetre(static_cast<float>(windowWidth + placeBoutton), static_cast<float>(windowHeight));
	panneau = sf::FloatRect({ std::round((tailleFenetre.x - largeurPanneau) / 2.f), std::round((tailleFenetre.y - hauteur) / 2.f) },
		{ largeurPanneau, hauteur });

	const float x = panneau.position.x + padding;
	const float y = panneau.position.y;

	boutonFermer = sf::FloatRect({ panneau.position.x + largeurPanneau - padding - 32.f, y + 22.f }, { 32.f, 32.f });

	// général
	const float yGeneral = y + 106.f;
	for (size_t i = 0; i < general.size(); ++i)
	{
		float xCol = x + (i % 3) * (largeurColonne + gouttiere);
		float yLigne = yGeneral + (i / 3) * general[0]->hauteur();
		general[i]->placer({ xCol, yLigne }, largeurColonne);
	}

	// colonnes
	ySeparateur = yGeneral + hauteurGeneral + 12.f;
	const float yColonnes = yGeneral + hauteurGeneral + 24.f;
	zoneProies = sf::FloatRect({ x, yColonnes }, { largeurColonne, hauteurColonne });
	zonePredateurs = sf::FloatRect({ x + largeurColonne + gouttiere, yColonnes }, { largeurColonne, hauteurColonne });
	zoneHerbe = sf::FloatRect({ x + 2.f * (largeurColonne + gouttiere), yColonnes }, { largeurColonne, hauteurColonne });

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
	placerColonne(herbe, zoneHerbe);

	// bouton d'export, à gauche de la croix des graphiques
	boutonExporter = sf::FloatRect({ boutonFermerGraphique.position.x - 12.f - 150.f, boutonFermerGraphique.position.y }, { 150.f, 32.f });
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
		else if (touche->code == sf::Keyboard::Key::A)
			playAnimations = !playAnimations;
		else if (touche->code == sf::Keyboard::Key::G)
			afficherGrille = !afficherGrille;
		return false;
	}

	bool reset = false;

	// glissement des curseurs
	if (const auto* deplacement = event.getIf<sf::Event::MouseMoved>())
	{
		if (!ouvert)
			return false;
		sf::Vector2f souris = window.mapPixelToCoords(deplacement->position);
		for (auto* groupe : { &general, &proies, &predateurs, &herbe })
			for (auto& w : *groupe)
				if (w->glisser(souris) && w->resetRequis)
					reset = true;
		return reset;
	}

	if (const auto* relache = event.getIf<sf::Event::MouseButtonReleased>())
	{
		if (relache->button == sf::Mouse::Button::Left)
			for (auto* groupe : { &general, &proies, &predateurs, &herbe })
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
		if (vue != Vue::Aucune && boutonExporter.contains(souris))
		{
			exporterGraphique();
			return false;
		}
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

	for (auto* groupe : { &general, &proies, &predateurs, &herbe })
		for (auto& w : *groupe)
			if (w->appuyer(souris) && w->resetRequis)
				reset = true;
	return reset;
}

void CInterface::animer(float dt)
{
	for (auto* groupe : { &general, &proies, &predateurs, &herbe })
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

// le fichier est créé dans le dossier d'où le programme est lancé
void CInterface::exporterGraphique()
{
	const CGraphique* graphique = &courbeProies;
	const CGraphique* superposee = nullptr;
	std::string nomFichier = "proies.csv";
	std::string entete; // vide : titres des axes
	if (vue == Vue::Predateurs)
	{
		graphique = &courbePredateurs;
		nomFichier = "predateurs.csv";
	}
	else if (vue == Vue::LesDeux)
	{
		superposee = &courbePredateurs;
		nomFichier = "proies_predateurs.csv";
		entete = "Jours,Proies,Predateurs"; // les deux courbes ont le même titre d'axe ("Population")
	}
	else if (vue == Vue::Phases)
	{
		graphique = &phases;
		nomFichier = "phases.csv";
	}

	try
	{
		graphique->exporter(nomFichier, superposee, entete);
		messageExport = "Exporté dans " + nomFichier;
	}
	catch (const std::exception& e)
	{
		messageExport = e.what();
	}
	horlogeExport.restart();
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
	cible.draw(creerTexte(font, utf8("Espace : jour suivant"), 12, Theme::texteDiscret, { x + 14.f, windowHeight - 84.f }));
	cible.draw(creerTexte(font, utf8(afficherGrille ? "G : grille (on)" : "G : grille (off)"), 12, Theme::texteDiscret, { x + 14.f, windowHeight - 64.f }));
	cible.draw(creerTexte(font, utf8(playAnimations ? "A : animations (on)" : "A : animations (off)"), 12, Theme::texteDiscret, { x + 14.f, windowHeight - 44.f }));
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

	// colonnes proies / prédateurs / herbe
	dessinerColonne(cible, souris, proies, "Proies", Theme::proie, zoneProies);
	dessinerColonne(cible, souris, predateurs, "Prédateurs", Theme::predateur, zonePredateurs);
	dessinerColonne(cible, souris, herbe, "Herbe", Theme::herbe, zoneHerbe);

	// pied de page
	cible.draw(creerTexte(font, utf8("« Population initiale » relance la simulation   ·   Échap pour fermer"), 13, Theme::texteDiscret,
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

	// bouton d'export
	sf::ConvexShape fondExport = rectangleArrondi(boutonExporter.size, 8.f, boutonExporter.contains(souris) ? Theme::survol : Theme::carte);
	fondExport.setPosition(boutonExporter.position);
	cible.draw(fondExport);
	sf::Text texteExport = creerTexte(font, utf8("Exporter (CSV)"), 15, Theme::texte, { 0.f, boutonExporter.position.y + boutonExporter.size.y / 2.f });
	texteExport.setPosition({ std::round(boutonExporter.position.x + (boutonExporter.size.x - texteExport.getLocalBounds().size.x) / 2.f), texteExport.getPosition().y });
	cible.draw(texteExport);

	// message après un export, affiché 3 secondes sous le bouton
	if (!messageExport.empty() && horlogeExport.getElapsedTime().asSeconds() < 3.f)
	{
		sf::Text message = creerTexte(font, utf8(messageExport), 13, Theme::texteDiscret, { 0.f, boutonExporter.position.y + boutonExporter.size.y + 16.f });
		message.setPosition({ std::round(boutonExporter.position.x + boutonExporter.size.x - message.getLocalBounds().size.x), message.getPosition().y });
		cible.draw(message);
	}

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
