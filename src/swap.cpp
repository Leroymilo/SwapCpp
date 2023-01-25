#include <iostream>
#include <list>
#include <regex>
#include <filesystem>

#include <SFML/Graphics.hpp>

#include "gameplay/level.hpp"
#include "UI/menu.hpp"
#include "save.hpp"

int main()
{
    // SFML initialization stuff :
    // sf::RenderWindow window(sf::VideoMode(800, 800), "SWAP!");
    sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "SWAP!", sf::Style::Fullscreen);  // For fullscreen
    window.setVerticalSyncEnabled(true);
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
            int won = run(level_id, save.is_solved(level_id), &window, font, &nb_steps);

            if (won > 0)
            {
                std::cout << "level " << level_id << " won!" << std::endl;

                bool already_solved = save.is_solved(level_id);
                save.solve(level_id, nb_steps, (won == 2));
                save.write();

                if (! already_solved && ! save.is_solved(level_id + 1))
                {
                    std::regex exp("^level[0-9]{3}.json$");
                    bool next_level = false;
                    for (const auto & entry : std::filesystem::directory_iterator("levels"))
                    {
                        std::string dir_name = entry.path().string().substr(7);
                        if (std::regex_match(dir_name, exp))
                        {
                            int lvl_nb = std::stoi(dir_name.substr(5));
                            if (lvl_nb == level_id + 1)
                            {
                                next_level = true;

                                break;
                            }
                        }
                    }
                    
                    if (next_level) level_id ++;
                    else scene = "Select";
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