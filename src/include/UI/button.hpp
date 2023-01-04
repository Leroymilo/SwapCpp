#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>

struct Alignment
{
    int nb_h, i_h, d_h, nb_v, i_v, d_v;

    Alignment();
    Alignment(int nb_h, int i_h, int d_h, int nb_v, int i_v, int d_v);
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
        bool defined = false;
    
    public:
        bool on = false;
        bool prev_on = false;

        Button();
        Button(std::string texture_name, std::string text, Alignment, sf::RenderWindow*);
        void reshape();
        bool update();
        void draw(sf::Font);
        bool clicked();
};

#endif //BUTTON_H