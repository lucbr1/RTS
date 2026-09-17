#include <SFML/Graphics.hpp>
#include "Variable.hpp"
#include <iostream>
#include "Grille.hpp"

int main()
{
	Etape etape = PROIE_REPRODUCTION_SUR_CASE;

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
	sf::Text text(font);
	text.setString("0");
	text.setCharacterSize(40);
	text.setFillColor(sf::Color::White);
	text.setStyle(sf::Text::Bold);


	//redimensionnement de l'image pour qu'elle remplisse la fenêtre
	sprite.setScale({
		static_cast<float>(windowWidth) / texture.getSize().x,
		static_cast<float>(windowHeight) / texture.getSize().y
		});


	//creation des bouttons
	sf::RectangleShape bouton1({ static_cast<float>(placeBoutton), 60.f });
	bouton1.setPosition({ static_cast<float>(windowWidth), 200.f });
	bouton1.setFillColor(sf::Color::White);

	sf::Text texteBoutton1(font, "Proie 1", 30);
	texteBoutton1.setPosition({ static_cast<float>(windowWidth+15), 215.f });
	texteBoutton1.setFillColor(sf::Color::Black);

	sf::RectangleShape bouton2({ static_cast<float>(placeBoutton), 60.f });
	bouton2.setPosition({ static_cast<float>(windowWidth), 270.f });
	bouton2.setFillColor(sf::Color::White);

	sf::Text texteBoutton2(font, "Proie 2", 30);
	texteBoutton2.setPosition({ static_cast<float>(windowWidth + 15), 285.f });
	texteBoutton2.setFillColor(sf::Color::Black);


	CGrille grid(window);

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
				if (keyPressed->code == sf::Keyboard::Key::Space)
				{
					grid.gameLoopProie(etape);
				}
			}

			if (const auto* mouseButton =
				event->getIf<sf::Event::MouseButtonPressed>())
			{
				if (mouseButton->button == sf::Mouse::Button::Left)
				{
					sf::Vector2f souris = window.mapPixelToCoords(sf::Mouse::getPosition(window));

					if (bouton1.getGlobalBounds().contains(souris))
					{
						etape = PROIE_REPRODUCTION_SUR_CASE;
					}
					if (bouton2.getGlobalBounds().contains(souris))
					{
						etape = PROIE_REPRODUCTION_ALEATOIRE;
					}
				}
			}
		}
		text.setString(std::to_string(grid.getNombreProies()));

		window.clear(sf::Color(180, 180, 180));

		
		window.draw(sprite);

		grid.afficher();
		window.draw(bouton1);
		window.draw(texteBoutton1);
		window.draw(bouton2);
		window.draw(texteBoutton2);

		window.draw(text);


		window.display();
	}

	return 0;
}