#include "Graphique.hpp"
#include "Variable.hpp"
#include <algorithm>

CGraphique::CGraphique(sf::Color couleurLigne = sf::Color::Green)
    : couleurLigne(couleurLigne) {}

// Ajoute un point discret (abscisse, ordonnée)
void CGraphique::ajouterPoint(int x, int y) {
    abscisses.push_back(x);
    ordonnees.push_back(y);
}

// Efface toutes les données enregistrées
void CGraphique::effacer() {
    abscisses.clear();
    ordonnees.clear();
}

// Modifie la couleur de la courbe
void CGraphique::definirCouleur(sf::Color couleur) {
    couleurLigne = couleur;
}

// Dessine le graphique dans 'zoneAffichage' (coordonnées en pixels)
void CGraphique::dessiner(sf::RenderTarget& cible, float marge = 20.f) const {
    // 1. Calcul automatique de la zone disponible en évitant la zone des boutons
    float gauchePanneau = marge;
    float hautPanneau = marge;
    float largeurPanneau = std::max(1.f, static_cast<float>(windowWidth) - static_cast<float>(placeBoutton) - 2.f * marge);
    float hauteurPanneau = std::max(1.f, static_cast<float>(windowHeight) - 2.f * marge);

    // 2. Fond du panneau graphique
    sf::RectangleShape panneau(sf::Vector2f(largeurPanneau, hauteurPanneau));
    panneau.setPosition(sf::Vector2f(gauchePanneau, hautPanneau));
    panneau.setFillColor(sf::Color(25, 25, 30, 230));
    panneau.setOutlineThickness(1.5f);
    panneau.setOutlineColor(sf::Color(120, 120, 140));
    cible.draw(panneau);

    // Il faut au moins 2 points pour tracer une courbe
    if (abscisses.size() < 2) return;

    // 3. Calcul automatique des bornes min/max
    auto [itMinX, itMaxX] = std::minmax_element(abscisses.begin(), abscisses.end());
    auto [itMinY, itMaxY] = std::minmax_element(ordonnees.begin(), ordonnees.end());

    float minX = static_cast<float>(*itMinX);
    float maxX = static_cast<float>(*itMaxX);
    float minY = static_cast<float>(std::min(0, *itMinY));
    float maxY = static_cast<float>(*itMaxY);

    float etendueX = (maxX - minX > 0.f) ? (maxX - minX) : 1.f;
    float etendueY = (maxY - minY > 0.f) ? (maxY - minY) : 1.f;

    // 4. Zone intérieure de tracé (avec une seconde marge à l'intérieur du panneau)
    float margeInterne = 20.f;
    float gaucheTrace = gauchePanneau + margeInterne;
    float hautTrace = hautPanneau + margeInterne;
    float largeurTrace = std::max(1.f, largeurPanneau - 2.f * margeInterne);
    float hauteurTrace = std::max(1.f, hauteurPanneau - 2.f * margeInterne);

    // 5. Construction et rendu du VertexArray (SFML 3)
    sf::VertexArray sommets(sf::PrimitiveType::LineStrip, abscisses.size()); 

    for (size_t i = 0; i < abscisses.size(); ++i) {
        float normX = (static_cast<float>(abscisses[i]) - minX) / etendueX;
        float normY = (static_cast<float>(ordonnees[i]) - minY) / etendueY;

        float pixelX = gaucheTrace + normX * largeurTrace;
        float pixelY = hautTrace + hauteurTrace - normY * hauteurTrace;

        sommets[i] = sf::Vertex{ sf::Vector2f{pixelX, pixelY}, couleurLigne }; 
    }

    cible.draw(sommets);
}