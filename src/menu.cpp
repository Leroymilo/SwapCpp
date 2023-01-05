#include "UI/menu.hpp"

#include <filesystem>
#include <string>
#include <regex>
#include <iostream>

sf::Texture title;
Button start_;
Button exit_;

void draw_title(sf::RenderWindow* win_p, sf::Font font)
{
    win_p->clear(sf::Color(138, 208, 234));

    sf::RectangleShape title_rect;
    float W = title.getSize().x, H = title.getSize().y;
    title_rect.setSize(sf::Vector2f(0.5*W, 0.5*H));
    title_rect.setTexture(&title);
    float X = (win_p->getSize().x - title_rect.getSize().x) / 2;
    float Y = 0.1 * (win_p->getSize().y - title_rect.getSize().y);
    title_rect.setPosition(X, Y);
    win_p->draw(title_rect);

    start_.draw(font);
    exit_.draw(font);

    win_p->display();
}

int title_screen(sf::RenderWindow* win_p, sf::Font font)
{
    title.loadFromFile("assets/Menu/Title.png");
    start_ = Button("continue", "Start", Alignment(1, 0, 0, 7, 4, 0), win_p);
    exit_ = Button("exit", "Exit", Alignment(1, 0, 0, 7, 6, 0), win_p);

    // First draw
    draw_title(win_p, font);

    while (win_p->isOpen())
    {
        sf::Event evnt;
        if (win_p->pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
            {
                win_p->close();
                return 0;
            }
            
            else if (evnt.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                win_p->setView(sf::View(view));
                start_.reshape();
                exit_.reshape();
                draw_title(win_p, font);
                win_p->display();
            }
        }

        if (start_.clicked())
        {
            return 1;
        }
        else if (exit_.clicked())
        {
            return 0;
        }

        if (start_.update() || exit_.update())
        {
            draw_title(win_p, font);
        }
    }

    return 0;
}

LevelGrid::LevelGrid(sf::RenderWindow* win_p) : win_p(win_p)
{
    std::regex exp("^level[0-9]{3}$");

    for(auto& dir : std::filesystem::recursive_directory_iterator("levels"))
    {
        std::string dir_name = dir.path().string().substr(7);
        if (dir.is_directory() && std::regex_match(dir_name, exp))
        {
            int lvl_nb = std::stoi(dir_name.substr(5));
            if (lvl_nb < 1)
            {
                std::cout << "level number cannot be 0 or negative" << std::endl;
                continue;
            }
            Button new_button("level/locked", std::to_string(lvl_nb), Alignment(), win_p);
            levels[lvl_nb] = new_button;
        }
    }
}

void LevelGrid::reshape()
{
    W = 1;
    while (win_p->getSize().x - (W + 2) * button_size - (W + 1) * delta > 0)
    {
        W++;
    }
    W--;

    H = 1;
    while (win_p->getSize().y - (H) * button_size - (H - 1) * delta > 0)
    {
        H++;
    }
    H--;
    
    for (int y=0; y<H; y++)
    {
        for (int x=0; x<W; x++)
        {
            int lvl_id = 1 + page * W * H + y * W + x;

            auto button = levels.find(lvl_id);

            if (button == levels.end())
                continue;   // The level is not in the list
            
            button->second.set_alignment(Alignment(W, x, delta, H, y, delta));
        }
    }
}

bool LevelGrid::update()
{
    bool updated = false;

    for (int y=0; y<H; y++)
    {
        for (int x=0; x<W; x++)
        {
            int lvl_id = 1 + page * W * H + y * W + x;

            auto button = levels.find(lvl_id);

            if (button == levels.end())
                continue;   // The level is not in the list
            
            updated |= button->second.update();
        }
    }

    return updated;
}

void LevelGrid::draw(sf::Font font)
{
    std::cout << levels.size() << std::endl;

    for (int y=0; y<H; y++)
    {
        for (int x=0; x<W; x++)
        {
            int lvl_id = 1 + page * W * H + y * W + x;

            auto button = levels.find(lvl_id);

            if (button == levels.end())
                continue;   // The level is not in the list
            
            button->second.draw(font);
        }
    }
}

int LevelGrid::clicked()
{
    for (int y=0; y<H; y++)
    {
        for (int x=0; x<W; x++)
        {
            int lvl_id = 1 + page * W * H + y * W + x;

            auto button = levels.find(lvl_id);

            if (button == levels.end())
                continue;   // The level is not in the list
            
            if (button->second.clicked())
            {
                return lvl_id;
            }
        }
    }

    return 0;
}

void draw_levels(sf::RenderWindow* win_p, LevelGrid* lvl_g_p, sf::Font font)
{
    win_p->clear(sf::Color(20, 30, 200));

    lvl_g_p->draw(font);

    win_p->display();
}

int level_select(sf::RenderWindow* win_p, sf::Font font)
{
    LevelGrid level_grid(win_p);

    // First draw
    draw_levels(win_p, &level_grid, font);

    while (win_p->isOpen())
    {
        sf::Event evnt;
        if (win_p->pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
            {
                win_p->close();
                return 0;
            }
            
            else if (evnt.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                win_p->setView(sf::View(view));
                draw_levels(win_p, &level_grid, font);
            }
        }

        int clicked = level_grid.clicked();

        if (clicked > 0)
        {
            return clicked;
        }

        if (level_grid.update())
        {
            draw_levels(win_p, &level_grid, font);
        }
    
    }

    return 0;
}