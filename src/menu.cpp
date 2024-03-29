#include <filesystem>
#include <string>
#include <regex>
#include <iostream>
#include <cmath>

#include "globals.hpp"
#include "UI/menu.hpp"

sf::Texture title;
Button start_;
Button exit_;
Button settings;

void draw_title(sf::RenderWindow* win_p)
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

    start_.draw();
    exit_.draw();
    settings.draw();

    win_p->display();
}

int title_screen(sf::RenderWindow* win_p)
{
    title.loadFromFile("assets/Menu/Title.png");
    start_ = Button("continue", "Start", Alignment(1, 0, 0, 8, 4, 0), win_p);
    exit_ = Button("exit", "Exit", Alignment(1, 0, 0, 8, 6, 0), win_p);
    settings = Button("settings", "Settings", Alignment(1, 0, 0, 8, 8, 0), win_p);

    // First draw
    draw_title(win_p);

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

            else if (evnt.type == sf::Event::GainedFocus)
            {
                draw_title(win_p);
            }
            
            else if (evnt.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                win_p->setView(sf::View(view));
                start_.reshape();
                exit_.reshape();
                settings.reshape();
                draw_title(win_p);
            }
        }

        if (start_.clicked())
        {
            return 1;
        }
        else if (settings.clicked())
        {
            return 2;
        }
        else if (exit_.clicked())
        {
            return 0;
        }

        if (start_.update() || exit_.update() || settings.update())
        {
            draw_title(win_p);
        }
    }

    return 0;
}

LevelGrid::LevelGrid(sf::RenderWindow* win_p, Save* save_p, int level_id) : win_p(win_p), save_p(save_p)
{
    std::regex exp("^level[0-9]{3}.json$");

    for(auto& entry : std::filesystem::directory_iterator("levels"))
    {
        std::string dir_name = entry.path().string().substr(7);
        if (std::regex_match(dir_name, exp))
        {
            int lvl_nb = std::stoi(dir_name.substr(5));
            if (lvl_nb < 1)
            {
                std::cout << "level number cannot be 0 or negative" << std::endl;
                continue;
            }

            std::string texture_name;
            if (save_p->is_perfected(lvl_nb))
                texture_name = "perfected";
            else if (save_p->is_solved(lvl_nb))
                texture_name = "solved";
            else if (save_p->is_playable(lvl_nb))
                texture_name = "unlocked";
            else
                texture_name = "locked";
    
            Button new_button("level/" + texture_name, std::to_string(lvl_nb), Alignment(), win_p);
            new_button.font_size = 60;
            levels[lvl_nb] = new_button;
        }
    }

    right = Button("right", "", Alignment(), win_p);
    left = Button("left", "", Alignment(), win_p);

    exit_ = Button("exit", "Exit", Alignment(), win_p);

    page = (level_id - 1) / (W * H);
    reshape();
}

void LevelGrid::reshape()
{
    // W = 1;
    // while (W < 11 && win_p->getSize().x * 0.8 > (W + 2) * button_size + (W + 1) * delta)
    // {
    //     W++;
    // }
    // W--;

    // H = 1;
    // while (win_p->getSize().y * 0.8 > (H + 1) * button_size + H * delta)
    // {
    //     H++;
    // }
    // H--;

    // For now, it's 12 levels per page anyway :
    if (win_p->getSize().x >= win_p->getSize().y)
        W = 4, H = 3;
    else
        W = 3, H = 4;

    nb_pages = std::ceil((float)levels.rbegin()->first / ((float)W * (float)H));
    if (page >= nb_pages)
    {
        page = nb_pages - 1;
    }
    
    for (int y=0; y<H; y++)
    {
        for (int x=0; x<W; x++)
        {
            int lvl_id = 1 + page * W * H + y * W + x;

            auto button = levels.find(lvl_id);

            if (button == levels.end())
                continue;   // The level is not in the list
            
            button->second.set_alignment(Alignment(W, x, delta, H+1, y, delta));
        }
    }

    right.set_alignment(Alignment(W+2, W+1, delta, 1, 0, 0));
    left.set_alignment(Alignment(W+2, 0, delta, 1, 0, 0));

    float mult = levels[1].shape.y / exit_.shape.y;
    exit_.set_alignment(Alignment(1, 0, 0, mult*H, mult*H-1, delta));
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

    updated |= right.update();
    updated |= left.update();

    updated |= exit_.update();

    return updated;
}

void LevelGrid::draw(sf::RenderWindow* win_p)
{
    for (int y=0; y<H; y++)
    {
        for (int x=0; x<W; x++)
        {
            int lvl_id = 1 + page * W * H + y * W + x;

            auto button = levels.find(lvl_id);

            if (button == levels.end())
                continue;   // The level is not in the list
            
            button->second.draw();
            
            if (save_p->is_solved(lvl_id))
            {
                int steps = save_p->get_steps(lvl_id);
                sf::Rect<int> but_hb = button->second.hitbox;
                std::string text = "steps : " + std::to_string(steps);
                sf::Text step_disp(text, font.get_font(), 26);
                sf::FloatRect bounds = step_disp.getLocalBounds();
                step_disp.setPosition(but_hb.left + (but_hb.width - bounds.width)/2, but_hb.top + but_hb.height);
                win_p->draw(step_disp);
            }
        }
    }

    if (nb_pages > 1)
    {
        right.draw();
        left.draw();
    }

    exit_.draw();
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
            
            if (button->second.clicked() && save_p->is_playable(lvl_id))
            {
                return lvl_id;
            }
        }
    }

    if (nb_pages > 1)
    {
        if (right.clicked())
        {
            page = (page + 1) % nb_pages;
            return -1;
        }
        if (left.clicked())
        {
            page = (page + nb_pages - 1) % nb_pages;
            return -1;
        }
    }

    if (exit_.clicked())
    {
        return -2;
    }

    return 0;
}

void draw_levels(sf::RenderWindow* win_p, LevelGrid* lvl_g_p)
{
    win_p->clear(sf::Color(20, 30, 120));

    lvl_g_p->draw(win_p);

    win_p->display();
}

int level_select(sf::RenderWindow* win_p, Save* save_p, int level_id)
{
    std::cout << "level id : " << level_id << std::endl;
    LevelGrid level_grid(win_p, save_p, level_id);

    // First draw
    draw_levels(win_p, &level_grid);

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

            else if (evnt.type == sf::Event::GainedFocus)
            {
                draw_levels(win_p, &level_grid);
            }
            
            else if (evnt.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                win_p->setView(sf::View(view));
                level_grid.reshape();
                draw_levels(win_p, &level_grid);
            }

            else if (evnt.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    return 0;
                }
            }
        }

        int clicked = level_grid.clicked();

        if (clicked > 0)
        {
            return clicked;
        }
        else if (clicked == -2)
        {
            return 0;
        }
        else if (clicked == -1)
        {
            level_grid.reshape();
            draw_levels(win_p, &level_grid);
        }

        if (level_grid.update())
        {
            draw_levels(win_p, &level_grid);
        }
    
    }

    return 0;
}