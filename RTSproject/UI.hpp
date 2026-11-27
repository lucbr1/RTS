#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>
#include "Graphique.hpp"

// Palette de l'interface
namespace Theme
{
	inline const sf::Color fond = sf::Color(24, 26, 32);          // barre latérale
	inline const sf::Color panneau = sf::Color(32, 35, 42);       // fond du menu
	inline const sf::Color carte = sf::Color(41, 45, 54);         // fond des colonnes
	inline const sf::Color survol = sf::Color(54, 59, 71);        // ligne survolée
	inline const sf::Color texte = sf::Color(232, 234, 238);
	inline const sf::Color texteDiscret = sf::Color(140, 147, 160);
	inline const sf::Color interrupteurOff = sf::Color(78, 84, 98);
	inline const sf::Color general = sf::Color(96, 150, 240);     // bleu
	inline const sf::Color proie = sf::Color(102, 187, 106);      // vert
	inline const sf::Color predateur = sf::Color(229, 83, 83);    // rouge
	inline const sf::Color herbe = sf::Color(214, 178, 72);       // jaune paille
}

// convertit une chaîne UTF-8 (accents) en sf::String
sf::String utf8(const std::string& s);

// rectangle aux coins arrondis
sf::ConvexShape rectangleArrondi(sf::Vector2f taille, float rayon, sf::Color couleur);

// texte centré verticalement sur position.y
sf::Text creerTexte(const sf::Font& font, const sf::String& s, unsigned int taille, sf::Color couleur, sf::Vector2f position, bool gras = false);

// Élément du menu (interrupteur, curseur...)
class CWidget
{
protected:
	const sf::Font* font;
	sf::String libelle;
	sf::Color accent;
	sf::FloatRect zone;
	const bool* actif; // si non nul, l'élément est grisé et inactif quand *actif == false

public:
	bool resetRequis = false; // la grille doit être réinitialisée quand on change la valeur

	CWidget(const sf::Font& font, const std::string& libelle, sf::Color accent, const bool* actif = nullptr);
	virtual ~CWidget() = default;

	bool estActif() const { return !actif || *actif; }
	virtual float hauteur() const = 0;
	void placer(sf::Vector2f position, float largeur) { zone = sf::FloatRect(position, { largeur, hauteur() }); }

	virtual bool appuyer(sf::Vector2f souris) = 0;          // renvoie true si la valeur a changé
	virtual bool glisser(sf::Vector2f) { return false; }    // souris déplacée (bouton enfoncé)
	virtual bool relacher() { return false; }               // renvoie true si la valeur a changé pendant le glissement
	virtual void animer(float) {}
	virtual void dessiner(sf::RenderTarget& cible, sf::Vector2f souris) const = 0;
};

// Ligne : libellé à gauche, interrupteur à droite, liée à un booléen
class CInterrupteur : public CWidget
{
	bool* valeur;
	float curseur; // position animée du bouton rond (0 = off, 1 = on)

public:
	CInterrupteur(const sf::Font& font, const std::string& libelle, bool& valeur, sf::Color accent, bool resetRequis = false);
	float hauteur() const override { return 46.f; }
	bool appuyer(sf::Vector2f souris) override;
	void animer(float dt) override;
	void dessiner(sf::RenderTarget& cible, sf::Vector2f souris) const override;
};

// Curseur : libellé + valeur (+ interrupteur optionnel) en haut, piste glissable en dessous,
// lié à un entier, à un réel ou à un taux (double entre 0 et 1 affiché en %)
class CCurseur : public CWidget
{
	unsigned int* entier = nullptr;
	double* reel = nullptr;
	double echelle = 1.0;     // valeur affichée = *reel * echelle (100 pour un taux)
	unsigned int min, max;
	unsigned int pas;         // la valeur avance de pas en pas (ex : 10 en 10)
	std::string unite;
	bool enGlissement = false;
	bool modifie = false;     // la valeur a changé depuis l'appui
	unsigned int valeurGlissement = 0; // si resetRequis : valeur affichée pendant le glissement, appliquée au relâchement

	bool* interrupteur = nullptr; // si non nul : interrupteur à droite du libellé, qui active le curseur
	float animInter = 0.f;

	sf::FloatRect piste() const;
	sf::FloatRect zoneInterrupteur() const;
	bool majDepuisSouris(sf::Vector2f souris);
	unsigned int lire() const;
	void ecrire(unsigned int v);

public:
	// si resetRequis est vrai, la simulation n'est relancée qu'au relâchement de la souris
	CCurseur(const sf::Font& font, const std::string& libelle, unsigned int& valeur, unsigned int min, unsigned int max,
		sf::Color accent, const bool* actif = nullptr, const std::string& unite = " j", unsigned int pas = 1, bool resetRequis = false);
	// réel affiché comme un entier (pas de 1) entre min et max
	CCurseur(const sf::Font& font, const std::string& libelle, double& valeur, unsigned int min, unsigned int max,
		sf::Color accent, const bool* actif = nullptr, const std::string& unite = " j");
	CCurseur(const sf::Font& font, const std::string& libelle, double& taux, sf::Color accent, const bool* actif = nullptr);

	// ajoute un interrupteur sur la ligne du libellé ; le curseur n'est actif que s'il est allumé
	CCurseur& avecInterrupteur(bool& valeur);

	float hauteur() const override { return 60.f; }
	bool appuyer(sf::Vector2f souris) override;
	bool glisser(sf::Vector2f souris) override;
	bool relacher() override;
	void animer(float dt) override;
	void dessiner(sf::RenderTarget& cible, sf::Vector2f souris) const override;
};

using ListeWidgets = std::vector<std::unique_ptr<CWidget>>;

// Barre latérale (bouton menu, statistiques, boutons des graphiques), menu des paramètres et graphiques
class CInterface
{
	enum class Vue { Aucune, Proies, Predateurs, LesDeux, Phases }; // même ordre que les boutons

	const sf::Font& font;
	bool ouvert = false;
	Vue vue = Vue::Aucune;

	sf::FloatRect boutonMenu;
	sf::FloatRect boutonFermer;
	sf::FloatRect panneau;
	sf::FloatRect zoneProies;
	sf::FloatRect zonePredateurs;
	sf::FloatRect zoneHerbe;
	float ySeparateur = 0.f;

	sf::FloatRect boutonsGraphiques[4];
	sf::FloatRect panneauGraphique;
	sf::FloatRect boutonFermerGraphique;
	sf::FloatRect boutonExporter;
	std::string messageExport;   // affiché quelques secondes après un export
	sf::Clock horlogeExport;
	CGraphique courbeProies;
	CGraphique courbePredateurs;
	CGraphique phases;

	ListeWidgets general;
	ListeWidgets proies;
	ListeWidgets predateurs;
	ListeWidgets herbe;

	void calculerDisposition();
	void exporterGraphique(); // exporte le graphique affiché dans un fichier .csv
	void dessinerBarreLaterale(sf::RenderTarget& cible, sf::Vector2f souris, unsigned int nbProies, unsigned int nbPredateurs, unsigned int jour) const;
	void dessinerMenu(sf::RenderTarget& cible, sf::Vector2f souris) const;
	void dessinerColonne(sf::RenderTarget& cible, sf::Vector2f souris, const ListeWidgets& lignes, const std::string& titre, sf::Color accent, sf::FloatRect zone) const;
	void dessinerGraphique(sf::RenderTarget& cible, sf::Vector2f souris) const;

public:
	CInterface(const sf::Font& font);

	bool estOuvert() const { return ouvert; }
	void basculer();

	// ajoute l'état du jour aux graphiques
	void enregistrer(unsigned int jour, unsigned int nbProies, unsigned int nbPredateurs);
	void effacerGraphiques();

	// gère un événement, renvoie true si la grille doit être réinitialisée
	bool gererEvenement(const sf::Event& event, const sf::RenderWindow& window);
	void animer(float dt);
	void dessiner(sf::RenderTarget& cible, sf::Vector2f souris, unsigned int nbProies, unsigned int nbPredateurs, unsigned int jour) const;
};
