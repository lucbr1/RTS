#include "Graphique.hpp"
#include "UI.hpp"
#include <algorithm>
#include <cmath>

CGraphique::CGraphique(sf::Color couleurLigne, const std::string& titreX, const std::string& titreY)
	: couleurLigne(couleurLigne), titreX(titreX), titreY(titreY)
{
}

void CGraphique::ajouterPoint(int x, int y)
{
	abscisses.push_back(x);
	ordonnees.push_back(y);
}

void CGraphique::effacer()
{
	abscisses.clear();
	ordonnees.clear();
}

// pas de graduation « rond » (1, 2, 5, 10, 20, 50...) pour avoir environ 5 graduations
static int pasGraduation(int maximum)
{
	double brut = std::max(1, maximum) / 5.0;
	double puissance = std::pow(10.0, std::floor(std::log10(brut)));
	double f = brut / puissance;
	double pas = (f <= 1.0 ? 1.0 : f <= 2.0 ? 2.0 : f <= 5.0 ? 5.0 : 10.0) * puissance;
	return std::max(1, static_cast<int>(pas));
}

// segment épais (sf::Lines ne fait qu'un pixel de large)
static void ajouterSegment(sf::VertexArray& triangles, sf::Vector2f a, sf::Vector2f b, float epaisseur, sf::Color couleur)
{
	sf::Vector2f d = b - a;
	float longueur = std::sqrt(d.x * d.x + d.y * d.y);
	if (longueur == 0.f)
		return;
	sf::Vector2f n(-d.y / longueur * epaisseur / 2.f, d.x / longueur * epaisseur / 2.f);
	for (sf::Vector2f p : { a + n, a - n, b + n, a - n, b - n, b + n })
		triangles.append(sf::Vertex{ p, couleur });
}

void CGraphique::dessiner(sf::RenderTarget& cible, const sf::Font& font, sf::FloatRect zone, bool marquerDepart, const CGraphique* superposee) const
{
	std::vector<const CGraphique*> courbes = { this };
	if (superposee)
		courbes.push_back(superposee);

	// zone de tracé : place à gauche pour les graduations, en bas pour les graduations et le titre de l'axe X
	const sf::FloatRect trace({ zone.position.x + 70.f, zone.position.y + 10.f }, { zone.size.x - 90.f, zone.size.y - 66.f });
	const float gauche = trace.position.x;
	const float bas = trace.position.y + trace.size.y;

	if (abscisses.size() < 2)
	{
		sf::Text message = creerTexte(font, utf8("Pas encore de données : appuie sur Espace pour avancer"), 16, Theme::texteDiscret, {});
		message.setPosition({ std::round(zone.position.x + (zone.size.x - message.getLocalBounds().size.x) / 2.f), std::round(zone.position.y + zone.size.y / 2.f) });
		cible.draw(message);
		return;
	}

	int maxDonneesX = 0;
	int maxDonneesY = 0;
	for (const CGraphique* c : courbes)
	{
		for (int x : c->abscisses)
			maxDonneesX = std::max(maxDonneesX, x);
		for (int y : c->ordonnees)
			maxDonneesY = std::max(maxDonneesY, y);
	}
	const int pasX = pasGraduation(maxDonneesX);
	const int pasY = pasGraduation(maxDonneesY);
	const int maxX = pasX * static_cast<int>(std::ceil(maxDonneesX / static_cast<double>(pasX)));
	const int maxY = pasY * static_cast<int>(std::ceil(maxDonneesY / static_cast<double>(pasY)));

	auto versPixel = [&](int x, int y) {
		return sf::Vector2f(gauche + trace.size.x * x / std::max(1, maxX), bas - trace.size.y * y / std::max(1, maxY));
	};

	// graduations et quadrillage
	for (int y = 0; y <= maxY; y += pasY)
	{
		float py = versPixel(0, y).y;
		sf::RectangleShape ligne({ trace.size.x, 1.f });
		ligne.setPosition({ gauche, std::round(py) });
		ligne.setFillColor(Theme::survol);
		cible.draw(ligne);

		sf::Text valeur = creerTexte(font, std::to_string(y), 12, Theme::texteDiscret, {});
		valeur.setPosition({ std::round(gauche - 10.f - valeur.getLocalBounds().size.x), std::round(py) });
		cible.draw(valeur);
	}
	for (int x = 0; x <= maxX; x += pasX)
	{
		float px = versPixel(x, 0).x;
		sf::RectangleShape ligne({ 1.f, trace.size.y });
		ligne.setPosition({ std::round(px), trace.position.y });
		ligne.setFillColor(Theme::survol);
		cible.draw(ligne);

		sf::Text valeur = creerTexte(font, std::to_string(x), 12, Theme::texteDiscret, {});
		valeur.setPosition({ std::round(px - valeur.getLocalBounds().size.x / 2.f), std::round(bas + 16.f) });
		cible.draw(valeur);
	}

	// axes
	sf::RectangleShape axeX({ trace.size.x, 2.f });
	axeX.setPosition({ gauche, bas - 1.f });
	axeX.setFillColor(Theme::texteDiscret);
	cible.draw(axeX);
	sf::RectangleShape axeY({ 2.f, trace.size.y });
	axeY.setPosition({ gauche - 1.f, trace.position.y });
	axeY.setFillColor(Theme::texteDiscret);
	cible.draw(axeY);

	// titres des axes
	sf::Text texteX = creerTexte(font, utf8(titreX), 14, Theme::texte, {}, true);
	texteX.setPosition({ std::round(gauche + (trace.size.x - texteX.getLocalBounds().size.x) / 2.f), std::round(bas + 42.f) });
	cible.draw(texteX);

	sf::Text texteY = creerTexte(font, utf8(titreY), 14, Theme::texte, {}, true);
	texteY.setRotation(sf::degrees(-90.f));
	texteY.setPosition({ std::round(zone.position.x + 14.f), std::round(trace.position.y + (trace.size.y + texteY.getLocalBounds().size.x) / 2.f) });
	cible.draw(texteY);

	// courbes
	for (const CGraphique* c : courbes)
	{
		sf::VertexArray courbe(sf::PrimitiveType::Triangles);
		for (size_t i = 1; i < c->abscisses.size(); ++i)
			ajouterSegment(courbe, versPixel(c->abscisses[i - 1], c->ordonnees[i - 1]), versPixel(c->abscisses[i], c->ordonnees[i]), 2.5f, c->couleurLigne);
		cible.draw(courbe);
	}

	if (marquerDepart)
	{
		sf::CircleShape depart(6.f, 24);
		depart.setOrigin({ 6.f, 6.f });
		depart.setPosition(versPixel(abscisses.front(), ordonnees.front()));
		depart.setFillColor(Theme::panneau);
		depart.setOutlineThickness(2.f);
		depart.setOutlineColor(couleurLigne);
		cible.draw(depart);
	}

	// point actuel
	for (const CGraphique* c : courbes)
	{
		if (c->estVide())
			continue;
		sf::CircleShape actuel(5.f, 24);
		actuel.setOrigin({ 5.f, 5.f });
		actuel.setPosition(versPixel(c->abscisses.back(), c->ordonnees.back()));
		actuel.setFillColor(c->couleurLigne);
		actuel.setOutlineThickness(2.f);
		actuel.setOutlineColor(sf::Color::White);
		cible.draw(actuel);
	}
}
