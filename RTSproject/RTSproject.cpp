#include <SFML/Graphics.hpp>
#include "Variable.hpp"
#include <iostream>
#include "Grille.hpp"
#include <fstream>

int main()
{
	std::ofstream fichier;

	fichier.open("populationsProiesReproductions.csv");
	fichier << "Jour;Proies;\n";
	fichier << "0;" << popInitialeH << ";\n";


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
	sf::Text textProie(font);
	textProie.setString("0");
	textProie.setCharacterSize(40);
	textProie.setFillColor(sf::Color::White);
	textProie.setStyle(sf::Text::Bold);

	sf::Text textPredateur(font);
	textPredateur.setString("0");
	textPredateur.setCharacterSize(40);
	textPredateur.setFillColor(sf::Color::Red);
	textPredateur.setStyle(sf::Text::Bold);
	textPredateur.setPosition({ 0, 50.f });


	sf::Text texteJour(font);
	texteJour.setString("Jour : 0");
	texteJour.setCharacterSize(40);
	texteJour.setStyle(sf::Text::Bold);
	texteJour.setPosition({ 0, 100.f });
	texteJour.setFillColor(sf::Color::White);
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

	sf::RectangleShape bouton4({ static_cast<float>(placeBoutton - 8), 60.f });
	bouton4.setPosition({ static_cast<float>(windowWidth + 4), 410.f });
	bouton4.setFillColor(sf::Color::White);
	bouton4.setOutlineThickness(3.f);
	bouton4.setOutlineColor(sf::Color::Black);

	sf::Text texteBoutton4(font, "Predateur 2", 30);
	texteBoutton4.setPosition({ static_cast<float>(windowWidth + 15), 425.f });
	texteBoutton4.setFillColor(sf::Color::Black);

	

	CGrille grid(window);
	unsigned int jour = 0;

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
					grid.gameLoop();

					jour += t;

					fichier << jour << ";" << grid.getNbProies() << ";\n";
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

			if (bouton4.getGlobalBounds().contains(souris))
				bouton4.setFillColor(sf::Color(200, 200, 200));
			else
				bouton4.setFillColor(sf::Color::White);

			if (const auto* mouseButton =event->getIf<sf::Event::MouseButtonPressed>())
			{
				if (mouseButton->button == sf::Mouse::Button::Left)
				{

					if (bouton1.getGlobalBounds().contains(souris))
					{
						grid.setEtape(PROIE_REPRODUCTION_SUR_CASE);
						jour = 0;
					}
					if (bouton2.getGlobalBounds().contains(souris))
					{
						grid.setEtape(PROIE_REPRODUCTION_ALEATOIRE);
						jour = 0;
					}
					if (bouton3.getGlobalBounds().contains(souris))
					{
						grid.setEtape(PREDATEUR_SEUL_MORT_ALEATOIRE);
						jour = 0;
					}
					if (bouton4.getGlobalBounds().contains(souris))
					{
						grid.setEtape(PREDATEUR_SEUL_MORT_BASTON);
						jour = 0;
					}
				}
			}
		}
		textProie.setString(std::to_string(grid.getNbProies()));
		textPredateur.setString(std::to_string(grid.getNbPredateurs()));
		texteJour.setString("Jour: " + std::to_string(jour));

		window.clear(sf::Color(180, 180, 180));

		
		window.draw(sprite);

		grid.afficher();
		window.draw(bouton1);
		window.draw(texteBoutton1);
		window.draw(bouton2);
		window.draw(texteBoutton2);
		window.draw(bouton3);
		window.draw(texteBoutton3);
		window.draw(bouton4);
		window.draw(texteBoutton4);

		window.draw(textProie);
		window.draw(textPredateur);
		window.draw(texteJour);


		window.display();
	}

	fichier.close();
	return 0;
}