#include <SFML/Graphics.hpp>
#include "Variable.hpp"
#include <iostream>
#include "Grille.hpp"

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
	sf::RectangleShape bouton1({ static_cast<float>(placeBoutton-8), 60.f });
	bouton1.setPosition({ static_cast<float>(windowWidth+4), 200.f });
	bouton1.setFillColor(sf::Color::White);
	bouton1.setOutlineThickness(3.f);
	bouton1.setOutlineColor(sf::Color::Black);

	sf::Text texteBoutton1(font, "Proie 1", 30);
	texteBoutton1.setPosition({ static_cast<float>(windowWidth+15), 215.f });
	texteBoutton1.setFillColor(sf::Color::Black);

	sf::RectangleShape bouton2({ static_cast<float>(placeBoutton-8), 60.f });
	bouton2.setPosition({ static_cast<float>(windowWidth+4), 270.f });
	bouton2.setFillColor(sf::Color::White);
	bouton2.setOutlineThickness(3.f);
	bouton2.setOutlineColor(sf::Color::Black);

	sf::Text texteBoutton2(font, "Proie 2", 30);
	texteBoutton2.setPosition({ static_cast<float>(windowWidth + 15), 285.f });
	texteBoutton2.setFillColor(sf::Color::Black);


	sf::RectangleShape bouton3({ static_cast<float>(placeBoutton - 8), 60.f });
	bouton3.setPosition({ static_cast<float>(windowWidth + 4), 340.f });
	bouton3.setFillColor(sf::Color::White);
	bouton3.setOutlineThickness(3.f);
	bouton3.setOutlineColor(sf::Color::Black);

	sf::Text texteBoutton3(font, "Predateur 1", 30);
	texteBoutton3.setPosition({ static_cast<float>(windowWidth + 15), 355.f });
	texteBoutton3.setFillColor(sf::Color::Black);


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
					grid.gameLoopProie();
					grid.gameLoopPredateur();
				}
			}

			sf::Vector2f souris = window.mapPixelToCoords(sf::Mouse::getPosition(window));

			if (bouton1.getGlobalBounds().contains(souris))
				bouton1.setFillColor(sf::Color(200, 200, 200));
			else
				bouton1.setFillColor(sf::Color::White);

			if (bouton2.getGlobalBounds().contains(souris))
				bouton2.setFillColor(sf::Color(200, 200, 200));
			else
				bouton2.setFillColor(sf::Color::White);

			if (bouton3.getGlobalBounds().contains(souris))
				bouton3.setFillColor(sf::Color(200, 200, 200));
			else
				bouton3.setFillColor(sf::Color::White);

			if (const auto* mouseButton =event->getIf<sf::Event::MouseButtonPressed>())
			{
				if (mouseButton->button == sf::Mouse::Button::Left)
				{

					if (bouton1.getGlobalBounds().contains(souris))
					{
						grid.setEtape(PROIE_REPRODUCTION_SUR_CASE);
					}
					if (bouton2.getGlobalBounds().contains(souris))
					{
						grid.setEtape(PROIE_REPRODUCTION_ALEATOIRE);
					}
					if (bouton3.getGlobalBounds().contains(souris))
					{
						grid.setEtape(PREDATEUR_SEUL);
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
		window.draw(bouton3);
		window.draw(texteBoutton3);

		window.draw(text);


		window.display();
	}

	return 0;
}