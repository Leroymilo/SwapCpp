#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>

#include "globals.hpp"


class Button
{
    private:
        bool defined = false;

        sf::Texture *ref_texture_p;
        sf::Vector2i shape;
        sf::RectangleShape sprite;

        sf::Text text;
        
        sf::Vector2f pos;
        sf::Vector2f text_pos;
        sf::RenderWindow* ref_win_p;

        sf::FloatRect hitbox;

        sf::Image *ref_image_p;
        bool hovered();

    protected:
        sf::Vector2i rect_pos;
        
        int state = 0;
        int prev_state = 0;
        
        std::string string;

        void update_string(std::string);

    public:
        Button();
        Button(
            sf::Texture *, sf::IntRect, sf::Image *, sf::RenderWindow*,
            sf::Vector2f scale, sf::Vector2f pos,
            std::string = ""
        );

        sf::Vector2i get_shape();
        void reshape(sf::Vector2f scale, sf::Vector2f pos);
        bool update();
        void draw();
        bool clicked();
};

class CycleButton: private Button
{
    private:
        sf::Texture texture;
        sf::Image image;
        std::vector<std::string> strings;
        int nb_sides;
        int side;

    public:
        CycleButton();
        CycleButton(
            std::string source_name, sf::RenderWindow*,
            sf::Vector2f scale, sf::Vector2f pos, int = 0
        );

        bool clicked();

        int get_state();
};

#endif //BUTTON_H