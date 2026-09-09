#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({ 800, 600 }),
        "Simulation proie-predateur"
    );

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();

        sf::CircleShape proie(10.f);
        proie.setFillColor(sf::Color::Green);
        proie.setPosition({ 100.f, 100.f });

        window.draw(proie);

        window.display();
    }

    return 0;
}