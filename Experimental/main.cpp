#include <SFML/Graphics.hpp>
#include "level.hpp"
#include <iostream>

bool undo()
{
}

int main()
{
    //SFML stuff and level initialization :
    sf::RenderWindow window(sf::VideoMode(500, 500), "SWAP!");
    sf::Clock clock;
    Level level(1);

    //First draw :
    window.clear(sf::Color(0, 0, 120));
    level.display(&window);
    
    //Input handling :
    float time = clock.getElapsedTime().asSeconds();
    float deltaT = 0.15;
    int nbKeys = 9;
    sf::Keyboard::Key keys[] = {sf::Keyboard::Key::Up, sf::Keyboard::Key::Right, sf::Keyboard::Key::Down, sf::Keyboard::Key::Left, 
    sf::Keyboard::Key::Space, sf::Keyboard::Key::BackSpace, sf::Keyboard::Key::Return, sf::Keyboard::Key::Add, sf::Keyboard::Key::Escape};
    std::string keysNames[] = {"Up", "Right", "Down", "Left", "Swap", "Undo", "Wait", "Restart", "Exit"};
    //Unused, just a description of the actions.
    bool keysStates[] = {false, false, false, false, false, false, false, false, false};
    int pkey = 0;

    //Gameplay variables :
    bool step = false;
    bool didSwap = false;
    char directions[] = {'U', 'R', 'D', 'L'};
    std::vector<Level> steps;

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
            else if (evnt.type == sf::Event::KeyPressed)
            {
                for (int i = 0; i < nbKeys; i++)
                {
                    if (sf::Keyboard::isKeyPressed(keys[i]))
                        keysStates[i] = true;
                }
            }
            else if (evnt.type == sf::Event::KeyReleased)
            {
                for (int i = 0; i < nbKeys; i++)
                {
                    if (!sf::Keyboard::isKeyPressed(keys[i]))
                        keysStates[i] = false;
                }
            }
        }

        //Remove input delay if prioritized key have changed :
        if (!keysStates[pkey] or pkey==9)
        {
            time -= deltaT;
            bool keyChanged = false;
            for (int i = 0; i < nbKeys; i++)
            {
                if (keysStates[i])
                {
                    pkey = i;
                    keyChanged = true;
                    break;
                }
            }
            if (!keyChanged)
                pkey = 9;
        }
        
        //Apply inputs :
        if (clock.getElapsedTime().asSeconds()-time > deltaT and pkey!=9)
        {
            step = false;
            didSwap = false;
            time = clock.getElapsedTime().asSeconds();
            if (pkey < 4)
                step = level.push(directions[pkey]);
            else if (pkey == 4)
            {
                step = level.swap();
                didSwap = true;
            }
            else if (pkey == 5 && steps.size() > 1)
            {
                steps.pop_back();
                level = steps[steps.size()-1];
            }
            else if (pkey == 6)
                step = true;

            if (step)
            {
                level.step(didSwap);
                steps.push_back(Level(level));
            }
        }

        window.clear(sf::Color(0, 0, 120));
        level.display(&window);
    }

    return 0;
}