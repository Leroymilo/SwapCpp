#include <SFML/Graphics.hpp>
#include "level.hpp"
#include <iostream>

int W = 500, H = 500;
float wasLeftPressed = false;

int main()
{
    sf::RenderWindow window(sf::VideoMode(500, 500), "SWAP!");
    Level level(1);
    window.clear(sf::Color(0, 0, 120));
    level.display(&window);
    sf::Clock clock;
    float time = 0;
    float deltaT = 0.2;
    char input = 'N';   // 'N' will be treated as None input

    bool step = false;
    bool didSwap = false;
    sf::Keyboard::Key prevKey;

    while (window.isOpen())
    {
        sf::Event evnt;
        if (window.pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
                window.close();
            
            else if (evnt.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                window.setView(sf::View(view));
            }
            else if (evnt.type == sf::Event::KeyReleased)
            {
                input = 'N';
                time -= deltaT;
            }
            else if (evnt.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
                {
                    input = 'U';
                    prevKey = sf::Keyboard::Key::Up;
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
                {
                    input = 'R';
                    prevKey = sf::Keyboard::Key::Right;
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
                {
                    input = 'D';
                    prevKey = sf::Keyboard::Key::Down;
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
                {
                    input = 'L';
                    prevKey = sf::Keyboard::Key::Left;
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
                {
                    input = ' ';
                    prevKey = sf::Keyboard::Key::Space;
                }
            }
        }
        

        //Avoiding multiple inputs in a row :
        if (clock.getElapsedTime().asSeconds()-time > deltaT)
        {

            step = false;
            didSwap = false;
            if (input != 'N')
            {
                time = clock.getElapsedTime().asSeconds();
                step = level.input(input);
                if (input == ' ')
                    didSwap = true;
            }

            if (step && !didSwap)
            {
                level.step();
            }
        }

        window.clear(sf::Color(0, 0, 120));
        level.display(&window);
    }

    return 0;
}