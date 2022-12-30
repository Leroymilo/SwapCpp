#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>

struct Alignment
{
    int nb_h, i_h, d_h, nb_v, i_v, d_v;

    Alignment(int, int, int, int, int, int);
    sf::Vector2i compute(sf::Vector2i size, sf::Vector2u win_size);
};

class Button
{
    private:
        sf::Texture sprite_on;
        sf::Texture sprite_off;
        std::string text;
        Alignment alignment;
        sf::Rect<int> hitbox;
        sf::RenderWindow* ref_win_p;
    
    public:
        bool on = false;
        bool prev_on = false;

        Button(std::string texture_name, std::string text, Alignment, sf::RenderWindow*);
        void reshape();
        bool update();
        void draw(sf::Font);
        bool clicked();
};

#endif //BUTTON_H