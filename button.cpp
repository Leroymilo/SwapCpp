#include <cmath>
#include <iostream>

#include "button.hpp"


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


Button::Button(std::string texture_name, std::string text, Alignment alignment, sf::RenderWindow* win_p) :
    text(text), alignment(alignment), ref_win_p(win_p)
{
    sprite_on.loadFromFile("assets/Menu/" + texture_name + "_on.png");
    sprite_off.loadFromFile("assets/Menu/" + texture_name + "_off.png");

    this->reshape();

    if (sprite_off.getSize() != sprite_on.getSize())
    {
        std::cout << "different on/off sizes for button " << texture_name << std::endl;
    }
}

void Button::reshape()
{
    sf::Vector2i box_size = sf::Vector2i(sprite_off.getSize());
    sf::Vector2i top_left = alignment.compute(box_size, ref_win_p->getSize());
    hitbox = sf::Rect<int>(top_left, box_size);
}

bool Button::update()
{
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*ref_win_p);
    prev_on = on;

    on = (hitbox.contains(mouse_pos) && sf::Mouse::isButtonPressed(sf::Mouse::Left));

    return (on != prev_on);
}

void Button::draw(sf::Font font)
{
    sf::RectangleShape button;
    if (on)
    {
        button.setSize(sf::Vector2f(sprite_on.getSize()));
        button.setTexture(&sprite_on);
    }
    else
    {
        button.setSize(sf::Vector2f(sprite_off.getSize()));
        button.setTexture(&sprite_off);
    }
    button.setPosition(sf::Vector2f(hitbox.left, hitbox.top));
    ref_win_p->draw(button);

    sf::Text text_disp;
    text_disp.setFont(font);
    text_disp.setString(text);
    float W = text_disp.getLocalBounds().width;
    float H = text_disp.getLocalBounds().height;
    text_disp.setPosition(sf::Vector2f(
        hitbox.left + (hitbox.width - W)/2,
        hitbox.top + (hitbox.height - H)/2
    ));
    ref_win_p->draw(text_disp);
}

bool Button::clicked()
{
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*ref_win_p);
    if (prev_on && !on && hitbox.contains(mouse_pos))
    {
        return true;
    }
    return false;
}