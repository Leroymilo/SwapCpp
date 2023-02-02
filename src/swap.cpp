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
            std::string map_res[3] = {"Title", "Select", "Settings"};
            scene = map_res[res];
        }

        else if (scene == "Settings")
        {
            // Into the settings screen :
            int res = settings(&window, &save, font);
            scene = "Title";
        }

        else if (scene == "Select")
        {
            // Selecting a level :
            int res = level_select(&window, &save, font);
            std::string map_res[2] = {"Title", "Play"};
            scene = map_res[res];
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

        }
    }
    
    save.write();
    return 0;
}