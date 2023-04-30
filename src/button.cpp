#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <json/value.h>
#include <json/json.h>

#include "UI/button.hpp"


Alignment::Alignment()
{
    nb_h = 1, i_h = 0, d_h = 0, nb_v = 1, i_h = 0, d_h = 0;
}

Alignment::Alignment(int nb_h, int i_h, int d_h, int nb_v, int i_v, int d_v) :
    nb_h(nb_h), i_h(i_h), d_h(d_h), nb_v(nb_v), i_v(i_v), d_v(d_v) {}

sf::Vector2i Alignment::compute(sf::Vector2i size, sf::Vector2u win_size)
{
    int w = size.x, h = size.y;
    int win_w = win_size.x, win_h = win_size.y;
    float left = (win_w - nb_h * w - (nb_h-1) * d_h) / 2 + i_h * (w + d_h);
    float top  = (win_h - nb_v * h - (nb_v-1) * d_v) / 2 + i_v * (h + d_v);
    return sf::Vector2i(std::round(left), std::round(top));
}

std::string Alignment::print()
{
    return std::to_string(i_h) + "/" + std::to_string(nb_h) + ", " + std::to_string(i_v) + "/" + std::to_string(nb_v);
}


Button::Button() {}

Button::Button(std::string texture_name, std::string text, Alignment alignment, sf::RenderWindow* win_p) :
    text(text), alignment(alignment), ref_win_p(win_p)
{
    defined = true;

    std::string file_name = "assets/Menu/" + texture_name + ".png";

    sf::Texture spritesheet;
    spritesheet.loadFromFile(file_name);
    int W = spritesheet.getSize().x, H = spritesheet.getSize().y;

    if (H%3 != 0)
    {
        std::cout << "texture not normalized for button " << text << " (filename : " << file_name << ")" << std::endl;
    }
    H /= 3;

    sprite_off.loadFromFile(file_name, sf::IntRect(0, 0, W, H));
    sprite_hover.loadFromFile(file_name, sf::IntRect(0, H, W, H));
    sprite_on.loadFromFile(file_name, sf::IntRect(0, 2*H, W, H));

    reshape();
}

void Button::reshape()
{
    if (!defined)
    {
        std::cout << "button \"" << text << "\" not defined, cannot reshape." << std::endl;
        return;
    }
    
    sf::Vector2i box_size = sf::Vector2i(sprite_off.getSize());
    sf::Vector2i top_left = alignment.compute(box_size, ref_win_p->getSize());
    hitbox = sf::Rect<int>(top_left, box_size);
}

bool Button::update()
{
    if (!defined)
    {
        std::cout << "button \"" << text << "\" not defined, cannot update." << std::endl;
        return false;
    }

    prev_on = on;
    prev_hover = hover;

    if (ref_win_p->hasFocus())
    {
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(*ref_win_p);
        hover = hitbox.contains(mouse_pos);
        on = (hover && sf::Mouse::isButtonPressed(sf::Mouse::Left));
    }
    else
    {
        on = false;
        hover = false;
    }

    return (on != prev_on) || (hover != prev_hover);
}

void Button::draw()
{
    if (!defined)
    {
        std::cout << "button \"" << text << "\" not defined, cannot draw." << std::endl;
        return;
    }

    sf::RectangleShape button;
    button.setSize(sf::Vector2f(sprite_on.getSize()));
    if (on)
    {
        button.setTexture(&sprite_on);
    }
    else if (hover)
    {
        button.setTexture(&sprite_hover);
    }
    else
    {
        button.setTexture(&sprite_off);
    }
    button.setPosition(sf::Vector2f(hitbox.left, hitbox.top));
    ref_win_p->draw(button);

    sf::Text text_disp(text, font.get_font());
    sf::FloatRect bounds = text_disp.getLocalBounds();
    text_disp.setPosition(sf::Vector2f(
        hitbox.left + (hitbox.width - bounds.width - bounds.left)/2,
        hitbox.top + (hitbox.height - bounds.height - bounds.top)/2
    ));
    ref_win_p->draw(text_disp);
}

bool Button::clicked()
{
    if (!defined)
    {
        std::cout << "button \"" << text << "\" not defined, cannot click." << std::endl;
        return false;
    }

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*ref_win_p);
    return (prev_on && !on && hitbox.contains(mouse_pos));
}

void Button::set_alignment(Alignment new_align)
{
    alignment = new_align;
    reshape();
}

CycleButton::CycleButton() {}

CycleButton::CycleButton(std::string key_word, Alignment alignment, sf::RenderWindow* win_p, int state) : state(state)
{
    // Reading Json :
    std::ifstream file("assets/Menu/cycle_buttons/" + key_word + ".json");
    Json::Reader reader;
    Json::Value json_data;
    reader.parse(file, json_data);

    nb_buttons = json_data["nb_states"].asInt();
    buttons.resize(nb_buttons);

    for (int i=0; i < nb_buttons; i++)
    {
        Json::Value state_data = json_data["states"][i];
        std::string file_name = state_data["file"].asCString();
        buttons[i] = Button("cycle_buttons/" + file_name, state_data["text"].asCString(), alignment, win_p);
    }

    hitbox = buttons[0].hitbox;
    defined = true;

    for (int i=1; i < nb_buttons; i++)
    {
        if (buttons[i].hitbox != hitbox)
        {
            std::cout << "CycleButton " << key_word << " state " << i << " does not have the same size as state 0!" << std::endl;
            defined = false;
        }
    }
    
    reshape();
}

int CycleButton::get_state()
{
    return state;
}

void CycleButton::reshape()
{
    if (!defined)
    {
        std::cout << "CycleButton button not defined, cannot reshape." << std::endl;
        return;
    }

    for (int i=0; i < nb_buttons; i++)
    {
        buttons[i].reshape();
    }
    hitbox = buttons[0].hitbox;
}

bool CycleButton::update()
{
    if (!defined)
    {
        std::cout << "CycleButton button not defined, cannot update." << std::endl;
        return false;
    }

    if (buttons[state].clicked())
    {
        state = (state + 1)%nb_buttons;
        buttons[state].update();
        return 1;
    }
    return buttons[state].update();
}

void CycleButton::draw()
{
    if (!defined)
    {
        std::cout << "CycleButton button not defined, cannot draw." << std::endl;
        return;
    }
    
    buttons[state].draw();
}

void CycleButton::set_alignment(Alignment new_align)
{
    for (auto &button : buttons)
        button.set_alignment(new_align);
}