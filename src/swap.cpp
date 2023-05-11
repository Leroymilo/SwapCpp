#include <iostream>
#include <list>
#include <regex>
#include <filesystem>

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
        // This is probably somewhat fucked because of my weird static building setup...
    }
    else
    {
        window.create(sf::VideoMode(1000, 900), "SWAP!");
    }

    window.setVerticalSyncEnabled(save.get_flag_state("vsync"));
    
    std::string scene = "Title";
    int level_id = 1;

    // Menu navigation loop :
    while (window.isOpen())
    {
        if (scene == "Title")
        {
            // Title screen logic :
            int res = Title(&window).run();
            if (res == 0)
            {
                window.close();
            }
            std::string map_res[] = {"Title", "Select", "Settings"};
            scene = map_res[res];
        }

        else if (scene == "Settings")
        {
            // Into the settings screen :
            int res = settings(&window, save);
            scene = "Title";
        }

        else if (scene == "Select")
        {
            // Selecting a level :
            int res = LevelSelect(&window, &save, level_id).run();
            if (res == 0)
                scene = "Title";
            else
            {
                scene = "Play";
                level_id = res;
            }
        }

        else if (scene == "Play")
        {
            // Running a level :
            int nb_steps;
            int won = run(level_id, save.is_solved(level_id), &window, &nb_steps);

            if (won > 0)
            {
                std::cout << "level " << level_id << " won!" << std::endl;

                save.solve(level_id, nb_steps, (won == 2));
                save.write();

                if (! save.is_solved(level_id + 1))
                {
                    std::stringstream ss;
                    ss << std::setfill('0') << std::setw(3) << level_id+1;
                    if (std::filesystem::exists("levels/level" + ss.str() + ".json") && (level_id%12 > 0))
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