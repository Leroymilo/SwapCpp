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
    int button_height = button.hitbox.height;
    sf::Vector2i top_left = text_align.compute(sf::Vector2i(button_height, button_height), ref_win_p->getSize());
    sf::Text text_disp(text, font, 30);
    sf::FloatRect bounds = text_disp.getLocalBounds();
    top_left.y += (button_height-bounds.height-bounds.top)/2;
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
        sf::Text desc(save_p->get_flag_desc(key), font, 30);
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

    exit_ = Button(
        "exit", "Exit",
        Alignment(2, 0, 32, flags.size()+2, flags.size()+1, 16),
        ref_win_p
    );
    apply = Button(
        "continue", "Apply",
        Alignment(2, 1, 32, flags.size()+2, flags.size()+1, 16),
        ref_win_p
    );

    reshape();
}

void Options::reshape()
{
    for (auto& line_elt : lines)
    {
        line_elt.second.reshape();
    }

    apply.reshape();
    exit_.reshape();
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

    updated |= exit_.update();
    updated |= apply.update();

    return updated;
}

void Options::draw(sf::Font font)
{
    ref_win_p->clear(sf::Color(20, 30, 200));
    for (auto& line_elt : lines)
    {
        line_elt.second.draw(font);
    }

    exit_.draw(font);
    apply.draw(font);

    ref_win_p->display();
}

int settings(sf::RenderWindow* win_p, Save* save_p, sf::Font font)
{
    bool prev_fs = save_p->get_flag_state("fullscreen");
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
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                win_p->setView(sf::View(view));
                opts.reshape();
                opts.draw(font);
            }

            else if (evnt.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    return 0;
                }
            }
        }

        if (opts.update())
        {
            opts.draw(font);
        }

        if (opts.apply.clicked())
        {
            if (save_p->get_flag_state("fullscreen") != prev_fs)
            {
                if (save_p->get_flag_state("fullscreen"))
                {
                    win_p->close();
                    win_p->create(sf::VideoMode::getFullscreenModes()[0], "SWAP!", sf::Style::Fullscreen);
                }
                else
                {
                    win_p->close();
                    win_p->create(sf::VideoMode(800, 800), "SWAP!");
                }
                prev_fs = save_p->get_flag_state("fullscreen");
                opts.reshape();
                opts.draw(font);
            }

            win_p->setVerticalSyncEnabled(save_p->get_flag_state("vsync"));
        }

        if (opts.exit_.clicked()) return 0;
    }
    return 0; 
}