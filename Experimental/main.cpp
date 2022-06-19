#include <SFML/Graphics.hpp>
#include "level.hpp"
#include <iostream>
#include <list>

int main()
{
    //SFML stuff and level initialization :
    sf::RenderWindow window(sf::VideoMode(500, 500), "SWAP!");
    sf::Clock clock;
    Level level(1);
    sf::Font font;
    if (!font.loadFromFile("assets\\font.ttf"))
        std::cout << "Could not load font" << std::endl;

    //First draw :
    window.clear(sf::Color(0, 0, 120));
    level.display(&window, font);
    
    //Input handling :
    int time = clock.getElapsedTime().asMilliseconds();
    int long_deltaT = 200;
    int short_deltaT = 50;
    bool long_press = false;
    bool process_input = false;
    int nbKeys = 9;
    sf::Keyboard::Key keys[] = {sf::Keyboard::Key::Up, sf::Keyboard::Key::Right, sf::Keyboard::Key::Down, sf::Keyboard::Key::Left, 
    sf::Keyboard::Key::Space, sf::Keyboard::Key::BackSpace, sf::Keyboard::Key::Return, sf::Keyboard::Key::Add, sf::Keyboard::Key::Escape};
    std::string keysNames[] = {"Up", "Right", "Down", "Left", "Swap", "Undo", "Wait", "Restart", "Exit"};//Unused, just a description of the actions.
    bool keysStates[] = {false, false, false, false, false, false, false, false, false};
    int pkey = 9;

    //Gameplay variables :
    bool step = false;
    bool didSwap = false;
    char directions[] = {'U', 'R', 'D', 'L'};
    std::list<Level> steps;
    steps.push_back(level);

    //Debug :
    int prev_event_time;

    while (window.isOpen())
    {
        // std::cout << "got into the main loop" << std::endl;
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

    //Manage input delay and changed keys :
        process_input = false;

        //Key was kept pressed :
        if (pkey!=9 and keysStates[pkey])
        {
            if (!long_press and clock.getElapsedTime().asMilliseconds() >= time + long_deltaT)
            {
                process_input = true;
                long_press = true;
                time = clock.getElapsedTime().asMilliseconds();
            }

            else if (long_press and clock.getElapsedTime().asMilliseconds() >= time + short_deltaT)
            {
                process_input = true;
                time = clock.getElapsedTime().asMilliseconds();
            }
        }

        //Key was unpressed :
        else
        {
            pkey = 9;
            long_press = false;
            for(int i = 0; i < nbKeys; i++)
            {
                if (keysStates[i])
                {
                    pkey = i;
                    process_input = true;
                    time = clock.getElapsedTime().asMilliseconds();
                    break;
                }
            }
        }
        
    //Apply inputs :
        if (pkey!=9 and process_input)
        {
            step = false;
            didSwap = false;
            if (pkey < 4)
            {
                step = level.push(directions[pkey]);
            }
            else if (pkey == 4)
            {
                step = level.swap();
                didSwap = true;
            }
            else if (pkey == 5 && steps.size() > 1)
            {
                steps.pop_back();
                level = steps.back();
            }
            else if (pkey == 6)
            {
                step = level.wait();
            }
            else if (pkey == 7)
            {
                level = steps.front();
                steps.push_back(level);
            }

            if (step)
            {
                level.step(didSwap);
                level.animate(&window, font, steps.back());
                steps.push_back(level);
            }
        }

        else
            level.display(&window, font);
    if (level.won)
        window.close();
    }

    return 0;
}