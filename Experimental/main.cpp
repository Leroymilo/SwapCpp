#include <SFML/Graphics.hpp>
#include "level.hpp"
#include <iostream>

int W = 500, H = 500;
float wasLeftPressed = false;

int main()
{
    sf::RenderWindow window(sf::VideoMode(500, 500), "SWAP!");
    Level level(2);

    while (window.isOpen())
    {
        sf::Event evnt;
        while (window.pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
                window.close();
            else if (evnt.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                window.setView(sf::View(view));
            }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            level.input('U');
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            level.input('R');
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            level.input('D');
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            level.input('L');
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
            level.input(' ');
        }

        window.clear(sf::Color(0, 0, 120));
        level.display(&window);
    }

    return 0;
}