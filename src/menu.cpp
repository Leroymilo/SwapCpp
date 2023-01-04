#include "UI/menu.hpp"

#include <filesystem>
#include <string>
#include <regex>

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
    std::regex exp("^levels(/|\\\\)level[0-9]{3}$");

    for(auto& dir : std::filesystem::recursive_directory_iterator("levels"))
    {
        std::string dir_name = dir.path().string().substr(7);
        if (dir.is_directory() && std::regex_match(dir_name, exp))
        {
            int lvl_nb = std::stoi(dir_name.substr(5));
            Button new_button("level/locked", std::to_string(lvl_nb), Alignment(), win_p);
            levels[lvl_nb] = new_button;
        }
    }
}

void LevelGrid::reshape()
{
    lvl_grid_w = 1;
    while (win_p->getSize().x - (lvl_grid_w + 2) * lvl_button_size - (lvl_grid_w + 1) * lvl_button_delta > 0)
    {
        lvl_grid_w++;
    }

    lvl_grid_h = 1;
    while (win_p->getSize().y - (lvl_grid_h) * lvl_button_size - (lvl_grid_h - 1) * lvl_button_delta > 0)
    {
        lvl_grid_h++;
    }

    
}

bool LevelGrid::update()
{

}

void LevelGrid::draw(sf::Font font)
{

}

int LevelGrid::clicked()
{
    return 0;
}

sf::Vector2i lvl_grid_size(sf::RenderWindow* win_p)
{

}

void draw_levels(sf::RenderWindow* win_p, sf::Font font)
{

    win_p->display();
}

int level_select(sf::RenderWindow* win_p, sf::Font font)
{

    LevelGrid level_grid(win_p);

    // First draw
    draw_title(win_p, font);

    while (win_p->isOpen())
    {
        sf::Event evnt;
        if (win_p->pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
            {
                return 0;
            }
            
            else if (evnt.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                win_p->setView(sf::View(view));
                draw_levels(win_p, font);
                win_p->display();
            }
        }
    
    }

    return 0;
}