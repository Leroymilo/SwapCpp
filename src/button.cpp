#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <json/value.h>
#include <json/json.h>

#include "UI/button.hpp"


Button::Button() {}

Button::Button(
    sf::Texture *texture_p, sf::IntRect texture_rect, sf::Image *image_p, sf::RenderWindow *win_p,
    sf::Vector2f scale, sf::Vector2f pos,
    std::string string
) :
    ref_texture_p(texture_p), ref_image_p(image_p), ref_win_p(win_p),
    string(string)
{
    rect_pos = sf::Vector2i(texture_rect.left, texture_rect.top);
    shape = sf::Vector2i(texture_rect.width, texture_rect.height);
    if (shape.y%3 != 0)
    {
        std::cout << "texture not normalized for button " << string << std::endl;
    }
    shape.y /= 3;

    reshape(scale, pos);
    
    defined = true;
}

sf::Vector2i Button::get_shape()
{
    return shape;
}

void Button::update_string(std::string new_string)
{
    string = new_string;

    text = font.size_text(string, sf::Vector2f(hitbox.width, hitbox.height));
    sf::FloatRect bounds = text.getGlobalBounds();
    text_pos = sf::Vector2f(
        hitbox.left + (hitbox.width - bounds.width - bounds.left)/2,
        hitbox.top + (hitbox.height - bounds.height - bounds.top)/2
    );
}

void Button::reshape(sf::Vector2f scale, sf::Vector2f pos)
{
    sprite.setScale(scale);
    sprite.setPosition(pos);
    
    hitbox = sprite.getGlobalBounds();

    if (string == "") return;

    update_string(string);
}

bool Button::hovered()
{
    sf::Vector2f mouse_pos = sf::Vector2f(sf::Mouse::getPosition(*ref_win_p));
    if (!hitbox.contains(mouse_pos)) return false;

    sf::Vector2f scale = sprite.getScale();

    int pxl_X = (mouse_pos.x - pos.x) / scale.x + rect_pos.x;
    int pxl_Y = (mouse_pos.y - pos.y) / scale.y + rect_pos.y;
    
    return (ref_image_p->getPixel(pxl_X, pxl_Y).a > 63);
    // Any pixel with alpha <= 1/4 is defined as "not clickable"
}

bool Button::update()
{
    if (!defined)
    {
        std::cout << "button \"" << string << "\" not defined, cannot update." << std::endl;
        return false;
    }

    prev_state = state;
    state = 0;

    if (ref_win_p->hasFocus())
    {
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(*ref_win_p);
        if (hovered()) state = 1;
        if (state==1 && sf::Mouse::isButtonPressed(sf::Mouse::Left)) state = 2;
    }

    return state != prev_state;
}

void Button::draw()
{
    if (!defined)
    {
        std::cout << "button \"" << string << "\" not defined, cannot draw." << std::endl;
        return;
    }

    sprite.setTexture(ref_texture_p);
    sprite.setTextureRect(sf::IntRect(
        rect_pos.x,     rect_pos.y + state * shape.y,
        shape.x,        shape.y
    ));
    ref_win_p->draw(sprite);
    ref_win_p->draw(text);
}

bool Button::clicked()
{
    if (!defined)
    {
        std::cout << "button \"" << string << "\" not defined, cannot click." << std::endl;
        return false;
    }

    return (prev_state == 2 && state < 2 && hovered());
}


CycleButton::CycleButton() {}

CycleButton::CycleButton(
    std::string source_name, sf::RenderWindow* win_p,
    sf::Vector2f scale, sf::Vector2f pos, int side
)
{
    // Reading Json :
    std::ifstream file("assets/Menu/cycle_buttons/" + source_name + ".json");
    Json::Reader reader;
    Json::Value json_data;
    reader.parse(file, json_data);

    nb_sides = json_data["nb_states"].asInt();
    this->side = side % nb_sides;

    strings.resize(nb_sides);
    for (int i=0; i < nb_sides; i++)
        strings[i] = json_data["states"][i].asCString();

    texture.loadFromFile(json_data["texture_path"].asCString());
    image = texture.copyToImage();

    int W = texture.getSize().x;
    if (W % nb_sides != 0) {
        std::cout << "texture not normalized for cycle button " << source_name << std::endl;
    }
    W /= nb_sides;

    Button(&texture, sf::IntRect(0, 0, W, texture.getSize().y), &image, win_p, scale, pos);
}

int CycleButton::get_state()
{
    return side;
}

bool CycleButton::clicked()
{
    bool was_clicked = Button::clicked();

    if (was_clicked) {
        side = (side + 1) % nb_sides;
        rect_pos.x = side * get_shape().x;
        update_string(strings[side]);
    }

    return was_clicked;
}