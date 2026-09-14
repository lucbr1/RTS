#include <SFML/Graphics.hpp>
#include "Variable.cpp"
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
	const sf::Texture texture("paysage.jpg");
	sf::Sprite sprite(texture);

	//redimensionnement de l'image pour qu'elle remplisse la fenêtre
	sprite.setScale({
		static_cast<float>(windowSize.x) / texture.getSize().x,
		static_cast<float>(windowSize.y) / texture.getSize().y
		});

	//calcule du nombre de ligne pour couvrir la fenetre et du nombre de vertex pour la grille
	unsigned int numLinesX = (windowSize.x / cellSize) + 1;
	unsigned int numLinesY = (windowSize.y / cellSize) + 1;
	unsigned int totalVertices = (numLinesX + numLinesY) * 2;

	//création d'un tableau de vertex pour la grille
	sf::VertexArray grid(sf::PrimitiveType::Lines, totalVertices);
	size_t vertexIndex = 0;

	//génération des lignes verticales
	for (size_t i = 0; i < numLinesX; ++i)
	{
		float posX = i * cellSize;
		grid[vertexIndex++] = sf::Vertex{ { posX, 0.f }, gridColor };
		grid[vertexIndex++] = sf::Vertex{ { posX, float(windowSize.y) }, gridColor };
	}

	//génération des lignes horizontales
	for (size_t j = 0; j < numLinesY; ++j)
	{
		float posY = j * cellSize;
		grid[vertexIndex++] = sf::Vertex{ { 0.f, posY }, gridColor };
		grid[vertexIndex++] = sf::Vertex{ { float(windowSize.x), posY }, gridColor };
	}

	while (window.isOpen())
	{
		//gestion des événements
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		window.clear();

		sf::CircleShape proie(10.f);
		proie.setFillColor(sf::Color::Green);
		proie.setPosition({ 100.f, 100.f });

		window.draw(sprite);

		window.draw(grid);

		window.draw(proie);

		window.display();
	}

	return 0;
}