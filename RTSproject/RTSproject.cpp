#include <SFML/Graphics.hpp>
#include "Proie.hpp"
#include "Variable.hpp"
#include "fonctions.hpp"
#include <iostream>

int main()
{
	//creation de la fenêtre
	sf::RenderWindow window(
		sf::VideoMode({ windowWidth, windowHeight }),
		"RTS"
	);

	sf::Vector2u windowSize = window.getSize();

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

	//calcule du nombre de ligne pour couvrir la fenetre et du nombre de vertex pour la grille
	unsigned int numLinesX = (windowWidth / cellSize) + 1;
	unsigned int numLinesY = (windowHeight / cellSize) + 1;
	unsigned int totalVertices = (numLinesX + numLinesY) * 2;

	//création d'un tableau de vertex pour la grille
	sf::VertexArray grid(sf::PrimitiveType::Lines, totalVertices);
	size_t vertexIndex = 0;

	//génération des lignes verticales
	for (size_t i = 0; i < numLinesX; ++i)
	{
		float posX = i * cellSize;
		grid[vertexIndex++] = sf::Vertex{ { posX, 0.f }, gridColor };
		grid[vertexIndex++] = sf::Vertex{ { posX, float(windowHeight) }, gridColor };
	}

	//génération des lignes horizontales
	for (size_t j = 0; j < numLinesY; ++j)
	{
		float posY = j * cellSize;
		grid[vertexIndex++] = sf::Vertex{ { 0.f, posY }, gridColor };
		grid[vertexIndex++] = sf::Vertex{ { float(windowWidth), posY }, gridColor };
	}

	std::vector<CProie*> proies = {};
	for (size_t i = 0; i < popInitialeH; ++i) {
		proies.push_back(new CProie());
	}

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
					gameLoopProie(window, proies);
				}
			}
		}
		text.setString(std::to_string(proies.size()));

		window.clear();

		
		window.draw(sprite);

		window.draw(grid);

		for (size_t i = 0; i < proies.size(); ++i) {
			proies[i]->afficher(window);
		}

		window.draw(text);


		window.display();
	}

	return 0;
}