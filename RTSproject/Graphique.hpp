#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// courbe y = f(x) tracée avec ses axes gradués (les axes partent de 0)
class CGraphique {
	std::vector<int> abscisses;
	std::vector<int> ordonnees;
	sf::Color couleurLigne;
	std::string titreX;
	std::string titreY;

public:
	CGraphique(sf::Color couleurLigne, const std::string& titreX, const std::string& titreY);

	void ajouterPoint(int x, int y);
	void effacer();
	bool estVide() const { return abscisses.empty(); }

	// marquerDepart : entoure le premier point (utile pour le diagramme de phases)
	// superposee : seconde courbe tracée sur les mêmes axes (ses titres d'axes sont ignorés)
	void dessiner(sf::RenderTarget& cible, const sf::Font& font, sf::FloatRect zone, bool marquerDepart = false, const CGraphique* superposee = nullptr) const;

	// entete : première ligne du fichier ; si vide, on utilise les titres des axes
	void exporter(const std::string& nomFichier, const CGraphique* superposee = nullptr, const std::string& entete = "") const;
};
