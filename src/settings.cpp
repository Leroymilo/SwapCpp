#include <iostream>

#include "globals.hpp"
#include "UI/settings.hpp"


Option_Line::Option_Line() {}

Option_Line::Option_Line(sf::RenderWindow* win_p, std::string key_word, std::string text, bool state, Alignment alignment) : text(text)
{
    ref_win_p = win_p;
    button = CycleButton(key_word, alignment, win_p, state);
    text_align = alignment;
    text_align.i_h = 0;
}

bool Option_Line::get_button_state()
{
    return button.get_state();
}

void Option_Line::reshape()
{
    button.reshape();
}

bool Option_Line::update()
{
    return button.update();
}

void Option_Line::draw()
{
    int button_height = button.hitbox.height;
    sf::Vector2i top_left = text_align.compute(sf::Vector2i(button_height, button_height), ref_win_p->getSize());
    sf::Text text_disp(text, font.get_font(), 30);
    sf::FloatRect bounds = text_disp.getLocalBounds();
    top_left.y += (button_height-bounds.height-bounds.top)/2;
    text_disp.setPosition(sf::Vector2f(top_left));
    ref_win_p->draw(text_disp);

    button.draw();
}


Options::Options() {}

Options::Options(Save* save_p, sf::RenderWindow* win_p)
{
    ref_save_p = save_p;
    ref_win_p = win_p;
    std::vector<std::string> flags = save_p->get_flags();

    // finding the longest text to align all buttons :
    float max_text_W = 0;
    for (std::string key : flags)
    {
        sf::Text desc(save_p->get_flag_desc(key), font.get_font(), 30);
        float width = desc.getLocalBounds().width;
        if (width > max_text_W)
        {
            max_text_W = width;
        }
    }

    // initializing lines :
    for (int i = 0; i < flags.size(); i++)
    {
        Alignment align(2, 1, max_text_W, flags.size(), i, DELTA);
        lines[flags[i]] = Option_Line(
            win_p,
            flags[i],
            save_p->get_flag_desc(flags[i]),
            save_p->get_flag_state(flags[i]),
            align
        );
    }

    exit_ = Button(
        "exit", "Exit",
        Alignment(2, 0, 32, flags.size()+2, flags.size()+1, DELTA),
        ref_win_p
    );
    apply = Button(
        "continue", "Apply",
        Alignment(2, 1, 32, flags.size()+2, flags.size()+1, DELTA),
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

void Options::draw()
{
    ref_win_p->clear(sf::Color(20, 30, 200));
    for (auto& line_elt : lines)
    {
        line_elt.second.draw();
    }

    exit_.draw();
    apply.draw();

    ref_win_p->display();
}

int settings(sf::RenderWindow* win_p, Save* save_p)
{
    Save save_copy = save_p->copy();

    bool prev_fs = save_copy.get_flag_state("fullscreen");
    Options opts(&save_copy, win_p);
    opts.draw();

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
                opts.draw();
            }
            
            else if (evnt.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                win_p->setView(sf::View(view));
                opts.reshape();
                opts.draw();
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
            opts.draw();
        }

        if (opts.apply.clicked())
        {
            if (save_copy.get_flag_state("fullscreen") != prev_fs)
            {
                if (save_copy.get_flag_state("fullscreen"))
                {
                    win_p->close();
                    win_p->create(sf::VideoMode::getFullscreenModes()[0], "SWAP!", sf::Style::Fullscreen);
                }
                else
                {
                    win_p->close();
                    win_p->create(sf::VideoMode(800, 800), "SWAP!");
                }
                prev_fs = save_copy.get_flag_state("fullscreen");
                opts.reshape();
                opts.draw();
            }

            win_p->setVerticalSyncEnabled(save_copy.get_flag_state("vsync"));

            save_p = &save_copy;
            save_p->write();
            save_copy = save_p->copy();
        }

        if (opts.exit_.clicked()) return 0;
    }
    return 0; 
}