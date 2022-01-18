#include <SFML/Graphics.hpp>
#include "level.hpp"
#include <iostream>

bool keyChanged(int n, bool * newK, bool * oldK)
{
    for (int i = 0; i < n; i++)
    {
        if (newK[i] != oldK[i])
            return true;
    }
    return false;
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
    sf::Keyboard::Key::Space, sf::Keyboard::Key::Return, sf::Keyboard::Key::Enter, sf::Keyboard::Key::Add, sf::Keyboard::Key::Escape};
    std::string keysNames[] = {"Up", "Right", "Down", "Left", "Swap", "Undo", "Wait", "Restart", "Exit"};
    bool keysStates[] = {false, false, false, false, false, false, false, false, false};
    bool oldKeysStates[] = {false, false, false, false, false, false, false, false, false};


    //Gameplay variables :
    bool step = false;
    bool didSwap = false;
    char directions[] = {'U', 'R', 'D', 'L'};

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

        //Remove input delay if keys have changed :
        if (keyChanged(nbKeys, oldKeysStates, keysStates))
        {
            time -= deltaT;
            for (int i = 0; i < nbKeys; i++)
            {
                oldKeysStates[i] = keysStates[i];
            }
        }
        
        //Apply inputs :
        if (clock.getElapsedTime().asSeconds()-time > deltaT)
        {
            step = false;
            didSwap = false;
            for (int i = 0; i < nbKeys; i++)
            {
                if (keysStates[i])
                {
                    time = clock.getElapsedTime().asSeconds();
                    if (i < 4)
                        step = level.push(directions[i]);
                    else if (i == 4)
                    {
                        // std::cout << "Player alive : " << Palive << std::endl;
                        // std::cout << "bullet alive : " << balive << std::endl;
                        step = level.swap();
                        didSwap = true;
                    }
                    break;
                }
            }
            if (step)
                level.step(didSwap);
        }

        window.clear(sf::Color(0, 0, 120));
        level.display(&window);
    }

    return 0;
}