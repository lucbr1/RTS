#include <SFML/Graphics.hpp>
#include "Variable.hpp"
#include <iostream>
#include "Grille.hpp"
#include "UI.hpp"
#include <map>

int main()
{
	//creation de la fenêtre
	sf::RenderWindow window(
		sf::VideoMode({ windowWidth+placeBoutton, windowHeight }),
		"RTS"
	);

	//chargement de l'image	
	sf::Texture texture;

	if (!texture.loadFromFile("assets/paysage.jpg"))
	{
		std::cerr << "Impossible de charger l'image\n";
	}
	sf::Sprite sprite(texture);


	sf::Font font;
	if (!font.openFromFile("assets/arial.ttf"))
	{
		std::cerr << "Impossible de charger la police\n";
	}
	//redimensionnement de l'image pour qu'elle remplisse la fenêtre
	sprite.setScale({
		static_cast<float>(windowWidth) / texture.getSize().x,
		static_cast<float>(windowHeight) / texture.getSize().y
		});


	CGrille grid(window);
	unsigned int jour = 0;

	CInterface ui(font);
	sf::Clock horlogeUI;

	while (window.isOpen())
	{
		//gestion des événements
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}

			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->code == sf::Keyboard::Key::Space && !ui.estOuvert())
				{
					for (size_t i = 0; i < t; ++i)
						grid.gameLoop();

					jour += t;
				}
			}

			// l'interface renvoie true si un paramètre demande de relancer la simulation
			if (ui.gererEvenement(*event, window))
			{
				grid.reset();
				jour = 0;
			}
		}
		ui.animer(horlogeUI.restart().asSeconds());

		sf::Vector2f souris = window.mapPixelToCoords(sf::Mouse::getPosition(window));

		window.clear(sf::Color(180, 180, 180));

		window.draw(sprite);
		grid.afficher();
		grid.afficherAnimations(font);

		ui.dessiner(window, souris, grid.getNbProies(), grid.getNbPredateurs(), jour);

		window.display();
	}

	return 0;
}
