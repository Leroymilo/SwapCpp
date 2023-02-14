#include <iostream>
#include <list>
#include <regex>
#include <filesystem>

#include <SFML/Graphics.hpp>

#include "gameplay/level.hpp"
#include "UI/menu.hpp"
#include "UI/settings.hpp"
#include "save.hpp"

int main()
{
    Save save(1);

    // SFML initialization stuff :
    sf::RenderWindow window;
    if (save.get_flag_state("fullscreen"))
    {
        window.create(sf::VideoMode::getFullscreenModes()[0], "SWAP!", sf::Style::Fullscreen);
        // The fullscreenMode index must be 2 on fedora linux to avoid weird mouse locking.
    }
    else
    {
        window.create(sf::VideoMode(800, 800), "SWAP!");
    }

    window.setVerticalSyncEnabled(save.get_flag_state("vsync"));

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
            std::vector<std::string> map_res = {"Title", "Select", "Settings"};
            scene = map_res[res];
            std::cout << "new scene : " << scene << std::endl;
        }

        else if (scene == "Settings")
        {
            // Into the settings screen :
            int res = settings(&window, &save, font);
            scene = "Title";
            std::cout << "new scene : " << scene << std::endl;
        }

        else if (scene == "Select")
        {
            // Selecting a level :
            int res = level_select(&window, &save, font);
            if (res == 0)
                scene = "Title";
            else
            {
                scene = "Play";
                level_id = res;
            }
            std::cout << "new scene : " << scene << std::endl;
        }

        else if (scene == "Play")
        {
            // Running a level :
            int nb_steps;
            int won = run(level_id, save.is_solved(level_id), &window, font, &nb_steps);

            if (won > 0)
            {
                std::cout << "level " << level_id << " won!" << std::endl;

                bool already_solved = save.is_solved(level_id);
                save.solve(level_id, nb_steps, (won == 2));
                save.write();

                if (! already_solved && ! save.is_solved(level_id + 1))
                {
                    std::stringstream ss;
                    ss << std::setfill('0') << std::setw(3) << level_id+1;
                    if (std::filesystem::exists("levels/level" + ss.str() + ".json"))
                    {
                        level_id++;
                    }
                    else
                    {
                        scene = "Select";
                    }
                }
                else scene = "Select";
            }
            else 
            {
                std::cout << "level exited" << std::endl;
                scene = "Select";
            }
            std::cout << "new scene : " << scene << std::endl;

        }
    }
    
    save.write();
    return 0;
}