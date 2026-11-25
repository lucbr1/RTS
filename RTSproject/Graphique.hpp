#include <SFML/Graphics.hpp>
#include <vector>

class CGraphique {
    std::vector<int> abscisses;
    std::vector<int> ordonnees;
    sf::Color couleurLigne;

public:
    CGraphique(sf::Color couleurLigne);

    void ajouterPoint(int x, int y);

    void effacer();

	void definirCouleur(sf::Color couleur);

	void dessiner(sf::RenderTarget& cible, float marge) const;   
};