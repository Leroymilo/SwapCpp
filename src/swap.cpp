#include <iostream>
#include <list>

#include <SFML/Graphics.hpp>

#include "gameplay/level.hpp"
#include "UI/menu.hpp"


int main()
{
    // SFML initialization stuff :
    sf::RenderWindow window(sf::VideoMode(800, 800), "SWAP!");
    sf::Font font;
    if (!font.loadFromFile("assets/font.ttf"))
        std::cout << "Could not load font" << std::endl;
    
    std::string scene = "Title";
    int level_id = 1;

    // Menu navigation loop :
    while (window.isOpen())
    {
        if (scene == "Title")
        {
            // Title screen logic :
            int res = title_screen(&window, font);
            if (res == 0)
            {
                window.close();
            }
            else
            {
                scene = "Select";
            }
        }

        else if (scene == "Select")
        {
            // Selecting a level :
            int res = level_select(&window, font);
            if (res == 0)
            {
                scene = "Title";
            }
            else
            {
                level_id = res;
                scene = "Play";
            }
        }

        else if (scene == "Play")
        {
            // Running a level :
            int won = run(level_id, &window, font);

            if (won == 1)
            {
                std::cout << "level " << level_id << " won!" << std::endl;
            }
            else 
            {
                std::cout << "level exited" << std::endl;
            }

            scene = "Select";
        }
    }
        
    return 0;
}