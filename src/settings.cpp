#include <iostream>

#include "UI/settings.hpp"

Option_Line::Option_Line() {}

Option_Line::Option_Line(sf::RenderWindow* win_p, std::string text, bool state, Alignment alignment) : text(text)
{
    ref_win_p = win_p;
    button = Toggle(alignment, win_p, state);
    text_align = alignment;
    text_align.i_h = 0;
}

bool Option_Line::get_button_state()
{
    return button.is_on();
}

void Option_Line::reshape()
{
    button.reshape();
}

bool Option_Line::update()
{
    return button.update();
}

void Option_Line::draw(sf::Font font)
{
    sf::Vector2i button_size(button.hitbox.width, button.hitbox.height);
    sf::Vector2i top_left = text_align.compute(button_size, ref_win_p->getSize());
    sf::Text text_disp(text, font, 20);
    sf::FloatRect bounds = text_disp.getLocalBounds();
    top_left.y += (top_left.y-bounds.height-bounds.top)/2;
    text_disp.setPosition(sf::Vector2f(top_left));
    ref_win_p->draw(text_disp);

    button.draw(font);
}


Options::Options() {}

Options::Options(Save* save_p, sf::RenderWindow* win_p, sf::Font font)
{
    ref_save_p = save_p;
    ref_win_p = win_p;
    std::vector<std::string> flags = save_p->get_flags();

    // finding the longest text to align all buttons :
    float max_text_W = 0;
    for (std::string key : flags)
    {
        sf::Text desc(save_p->get_flag_desc(key), font, 20);
        float width = desc.getLocalBounds().width;
        if (width > max_text_W)
        {
            max_text_W = width;
        }
    }

    // initializing lines :
    for (int i = 0; i < flags.size(); i++)
    {
        Alignment align(2, 1, max_text_W, flags.size(), i, 16);
        lines[flags[i]] = Option_Line(
            win_p,
            save_p->get_flag_desc(flags[i]),
            save_p->get_flag_state(flags[i]),
            align
        );
    }

    std::cout << "nb of options : " << flags.size() << std::endl;

    reshape();
}

void Options::reshape()
{
    for (auto& line_elt : lines)
    {
        line_elt.second.reshape();
    }
}

bool Options::update()
{
    bool updated = false;
    for (auto& line_elt : lines)
    {
        if (line_elt.second.update())
        {
            bool new_state = line_elt.second.get_button_state();
            ref_save_p->set_flag_state(line_elt.first, new_state);
            updated = true;
        }
    }

    return updated;
}

void Options::draw(sf::Font font)
{
    std::cout << "options in drawing..." << std::endl;
    ref_win_p->clear(sf::Color(20, 30, 200));
    for (auto& line_elt : lines)
    {
        line_elt.second.draw(font);
    }
}

int settings(sf::RenderWindow* win_p, Save* save_p, sf::Font font)
{
    Options opts(save_p, win_p, font);
    opts.draw(font);
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
                opts.draw(font);
            }
            
            else if (evnt.type == sf::Event::Resized)
            {
                opts.reshape();
                opts.draw(font);
            }
        }
    }
    return 0; 
}