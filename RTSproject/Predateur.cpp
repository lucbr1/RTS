#include "Predateur.hpp"
#include "fonctions.hpp"

CPredateur::CPredateur(int x, int y, unsigned int age) : CEntite(age) {
	unsigned int numLinesX = (windowWidth / cellSize) - 1;
	unsigned int numLinesY = (windowHeight / cellSize) - 1;

	position = sf::Vector2f(static_cast<float>(cellSize * x), static_cast<float>(cellSize * y));
	age = 0;
	tempsDeVie = 100;
}

void CPredateur::afficher(sf::RenderWindow& window) {
	sf::CircleShape predateur(cellSize / 4.0f);
	predateur.setFillColor(sf::Color::Red);
	predateur.setPosition(sf::Vector2f(static_cast<float>(position.x+cellSize/4), static_cast<float>(position.y+cellSize/4)));
	window.draw(predateur);
}

void CPredateur::seDeplacer() {
	//verifie si le predateur meurt maintenant
	if(randomizer(0, 100)/100.0 < tauxMortaliteC) // 5% de chance de mourir à chaque déplacement
	{
		// Le predateur meurt
		tempsDeVie = 0;
		return;
	}

	int maxX = (windowWidth / cellSize) - 1;
	int maxY = (windowHeight / cellSize) - 1;


	int dx = randomizer(-1, 1);
	int dy = randomizer(-1, 1);

	int x = static_cast<int>(position.x / cellSize);
	int y = static_cast<int>(position.y / cellSize);

	x += dx;
	y += dy;

	x = (x % (maxX + 1) + (maxX + 1)) % (maxX + 1);
	y = (y % (maxY + 1) + (maxY + 1)) % (maxY + 1);

	position.x = static_cast<float>(x * cellSize);
	position.y = static_cast<float>(y * cellSize);

	age++;
}
