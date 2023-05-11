#include <filesystem>
#include <string>
#include <regex>
#include <iostream>
#include <cmath>

#include "globals.hpp"
#include "UI/menu.hpp"

Title::Title(sf::RenderWindow* win_p) : ref_win_p(win_p)
{
    title_texture.loadFromFile("assets/Menu/Title.png");

    button_texture.loadFromFile("assets/Menu/title_buttons.png");
    button_image = button_texture.copyToImage();
    
    int W = button_texture.getSize().x, H = button_texture.getSize().y;
    if (W % 3 != 0)
        std::cout << "texture not normalized for title screen" << std::endl;
    
    W /= 3;

    start_   = Button(&button_texture, sf::IntRect(0*W, 0, W, H), &button_image, win_p, "Play");
    settings = Button(&button_texture, sf::IntRect(1*W, 0, W, H), &button_image, win_p, "Settings");
    exit_    = Button(&button_texture, sf::IntRect(2*W, 0, W, H), &button_image, win_p, "Quit");

    full_title_size.x = __max(title_texture.getSize().x + 2 * title_texture.getSize().y, 3 * W);
    // max( title width + a title height on each side, 3 button width )
    full_title_size.y = (1 + 3 * 0.5) * title_texture.getSize().y + (3 + 2) * H / 3;
    // title height + half a title height over and under title and under buttons + 3 buttons heights separated by a button height (twice cuz 3 buttons)
    // These proportions look alright for original textures sizes and are applied in `reshape`.

    reshape();
}

void Title::reshape()
{
    scale = __min(ref_win_p->getSize().x / full_title_size.x, ref_win_p->getSize().y / full_title_size.y);

    float Y_offset = (ref_win_p->getSize().y - scale * full_title_size.y) / 2;

    title_pos.x = (ref_win_p->getSize().x - scale * title_texture.getSize().x) / 2;
    title_pos.y = Y_offset + 0.5 * scale * title_texture.getSize().y;
    title_sprite.setPosition(title_pos);
    title_sprite.setScale(scale, scale);

    Button *buttons[] = {&start_, &settings, &exit_};
    sf::Vector2f pos;
    pos.x = (ref_win_p->getSize().x - scale * button_texture.getSize().x / 3);

    int min_font = INFINITY;
    for (int i = 0; i < 3; i++) {
        pos.y = Y_offset + scale * title_texture.getSize().y * 2 + scale * (i * 2 * button_texture.getSize().y / 3);
        buttons[i]->reshape(sf::Vector2f(scale, scale), pos);
        min_font = __min(min_font, buttons[i]->get_font_size());
    }

    for (Button *button_p : buttons) {
        button_p->set_font_size(min_font);
    }
}

void Title::draw()
{
    ref_win_p->clear(bg_color);

    title_sprite.setTexture(&title_texture);
    ref_win_p->draw(title_sprite);
    
    start_.draw();
    exit_.draw();
    settings.draw();

    ref_win_p->display();
}

int Title::run()
{
    // First draw
    reshape();
    draw();

    while (ref_win_p->isOpen())
    {
        sf::Event evnt;
        if (ref_win_p->pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
            {
                ref_win_p->close();
                return 0;
            }

            else if (evnt.type == sf::Event::GainedFocus)
            {
                draw();
            }
            
            else if (evnt.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                ref_win_p->setView(sf::View(view));
                reshape();
                draw();
            }
        }

        if (start_.update() || exit_.update() || settings.update())
        {
            draw();
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
    }

    return 0;
}

LevelSelect::LevelSelect(sf::RenderWindow* win_p, Save* save_p, int level_id) : ref_win_p(win_p), save_p(save_p)
{
    button_texture.loadFromFile("assets/Menu/level_select_buttons.png");
    button_image = button_texture.copyToImage();
    exit_texture.loadFromFile("assets/Menu/exit.png");
    exit_image = exit_texture.copyToImage();

    int W = button_texture.getSize().x, H = button_texture.getSize().y;
    if (W % 4 != 0 || H % 2 != 0)
        std::cout << "texture not normalized for level select screen" << std::endl;
    W /= 4;
    H /= 2;

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

            int lvl_stt = save_p->get_level_state(lvl_nb);
    
            Button new_button(
                &button_texture,
                sf::IntRect(W*(1+lvl_stt%2), H*(lvl_stt/2), W, H),
                &button_image,
                win_p,
                std::to_string(lvl_nb)
            );
            levels[lvl_nb] = new_button;
        }
    }

    right = Button(&button_texture, sf::IntRect(3*W, 0, W, 2*H), &button_image, win_p);
    left  = Button(&button_texture, sf::IntRect(0*W, 0, W, 2*H), &button_image, win_p);

    int w = exit_texture.getSize().x, h = exit_texture.getSize().y;
    exit_ = Button(&exit_texture, sf::IntRect(0, 0, w, h), &exit_image, win_p, "Exit");

    nb_pages = std::ceil((float)levels.rbegin()->first / 12);
    change_page((level_id - 1) / 12);

    full_grid_size.x = (2 + 4 + (5 + 2) * .5) * W;
    // (2 move + 4 levels) widths + 5 half widths between buttons + 2 half widths as margins
    full_grid_size.y = (3 + (2 + 2) * .5) * (H / 3) + (1 + 2 * .5) * (exit_texture.getSize().y / 3);
    // 3 level heights + 2 half heights between levels + 2 as margins + exit height + 2 half heights as margins around exit
    // These proportions look alright for original textures sizes and are applied in `reshape`.

    reshape();
}

void LevelSelect::change_page(int new_page)
{
    page = new_page;

    int W = 4, H = 3;
    current_levels.clear();
    for (int y=0; y<H; y++) {
        for (int x=0; x<W; x++) {
            int lvl_id = 1 + page * W * H + y * W + x;
            if (levels.find(lvl_id) == levels.end()) continue;
            current_levels.push_back(lvl_id);
        }
    }
}

void LevelSelect::reshape()
{
    int W = 4, H = 3;

    scale = __min(ref_win_p->getSize().x / full_grid_size.x, ref_win_p->getSize().y / full_grid_size.y);

    float Y_offset = (ref_win_p->getSize().y - scale * full_grid_size.y) / 2;
    float X_offset = (ref_win_p->getSize().x - scale * full_grid_size.x) / 2;

    sf::Vector2f pos;
    font_size = INFINITY;
    for (int y=0; y<H; y++) {
        for (int x=0; x<W; x++) {
            for (int z; z<nb_pages; z++) {
                int lvl_id = 1 + z * W * H + y * W + x;
                if (levels.find(lvl_id) == levels.end()) continue;

                pos.x = X_offset + scale * ((.5 + 1.5 * (1 + x)) * (button_texture.getSize().x / 4));
                pos.y = Y_offset + scale * ((.5 + 1.5 * y) * (button_texture.getSize().y / 6));
                levels[lvl_id].reshape(sf::Vector2f(scale, scale), pos);
                font_size = __min(font_size, levels[lvl_id].get_font_size());
            }
        }
    }

    for (int y=0; y<H; y++) {
        for (int x=0; x<W; x++) {
            for (int z; z<nb_pages; z++) {
                int lvl_id = 1 + z * W * H + y * W + x;
                if (levels.find(lvl_id) == levels.end()) continue;
                levels[lvl_id].set_font_size(font_size);
            }
        }
    }

    pos.y = Y_offset + scale * (1.5 * (button_texture.getSize().y / 6));
    pos.x = X_offset + scale * (.5 * (button_texture.getSize().x / 4));
    right.reshape(sf::Vector2f(scale, scale), pos);
    pos.x = X_offset + scale * ((.5 + 1.5 * 5) * (button_texture.getSize().x / 4));
    left.reshape(sf::Vector2f(scale, scale), pos);

    pos.x = (ref_win_p->getSize().x - scale * exit_texture.getSize().x) / 2;
    pos.y = Y_offset + scale * ((3 + 4 * .5) * (button_texture.getSize().y / 6) + .5 * (exit_texture.getSize().y / 3));
    exit_.reshape(sf::Vector2f(scale, scale), pos);

    ref_win_p->display();
}

bool LevelSelect::update()
{
    int W = 4, H = 3;
    bool updated = false;

    
    for (int lvl_id : current_levels)
        updated |= levels[lvl_id].update();

    updated |= right.update();
    updated |= left.update();

    updated |= exit_.update();

    return updated;
}

void LevelSelect::draw()
{
    int W = 4, H = 3;
    int lvl_W = button_texture.getSize().x / 4, lvl_H = button_texture.getSize().y / 6;

    for (int lvl_id : current_levels) {
        levels[lvl_id].draw();
        
        if (save_p->is_solved(lvl_id))
        {
            int steps = save_p->get_steps(lvl_id);
            std::string string = "steps : " + std::to_string(steps);
            sf::Text step_disp(string, font.get_font(), font_size/3);
            sf::FloatRect bounds = step_disp.getLocalBounds();

            sf::FloatRect but_hb = levels[lvl_id].get_hitbox();
            step_disp.setPosition(but_hb.left + (but_hb.width - bounds.width)/2, but_hb.top + but_hb.height);
            ref_win_p->draw(step_disp);
        }
    }

    if (nb_pages > 1)
    {
        right.draw();
        left.draw();
    }

    exit_.draw();
}

int LevelSelect::clicked()
{
    for (int lvl_id : current_levels) {
        
        if (levels[lvl_id].clicked() && save_p->get_level_state(lvl_id))
        {
            return lvl_id;
        }
    }

    if (nb_pages > 1)
    {
        if (right.clicked())
        {
            change_page((page + 1) % nb_pages);
            return -1;
        }
        if (left.clicked())
        {
            change_page((page + nb_pages - 1) % nb_pages);
            return -1;
        }
    }

    if (exit_.clicked())
    {
        return -2;
    }

    return 0;
}

int LevelSelect::run()
{
    // First draw
    draw();

    while (ref_win_p->isOpen())
    {
        sf::Event evnt;
        if (ref_win_p->pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
            {
                ref_win_p->close();
                return 0;
            }

            else if (evnt.type == sf::Event::GainedFocus)
            {
                draw();
            }
            
            else if (evnt.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                ref_win_p->setView(sf::View(view));
                reshape();
                draw();
            }

            else if (evnt.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    return 0;
                }
            }
        }

        if (update())
        {
            draw();
        }

        int clicked_val = clicked();

        if (clicked_val > 0)
        {
            return clicked_val;
        }
        else if (clicked_val == -2)
        {
            return 0;
        }
        else if (clicked_val == -1)
        {
            draw();
        }
    }

    return 0;
}