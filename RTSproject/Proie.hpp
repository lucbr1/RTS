#pragma once

#include <SFML/Graphics.hpp>
#include "Variable.hpp"

class CProie {

	sf::Vector2f position;
	unsigned int age;
	unsigned int tempsDeVie;

	public:
		CProie();
		void seDeplacer();
		std::optional<CProie*> seReproduire();
		void mourir();
		void afficher(sf::RenderWindow& window);
		sf::Vector2f getPosition() const { return position; }
		bool estVivant() const { return age < tempsDeVie; }
};