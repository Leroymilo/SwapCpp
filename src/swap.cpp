#include <iostream>
#include <list>

#include <SFML/Graphics.hpp>

#include "gameplay/level.hpp"
#include "UI/menu.hpp"
#include "save.hpp"

int main()
{
    // SFML initialization stuff :
    sf::RenderWindow window(sf::VideoMode(800, 800), "SWAP!");
    sf::Font font;
    if (!font.loadFromFile("assets/font.ttf"))
        std::cout << "Could not load font" << std::endl;
    
    Save save(1);
    
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
            int res = level_select(&window, &save, font);
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
            int nb_steps;
            int won = run(level_id, &window, font, &nb_steps);

            if (won == 1)
            {
                std::cout << "level " << level_id << " won!" << std::endl;
                save.solve(level_id, nb_steps);
            }
            else 
            {
                std::cout << "level exited" << std::endl;
            }

            scene = "Select";
        }
    }
    
    save.write();
    return 0;
}