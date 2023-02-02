#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>

struct Alignment
{
    int nb_h, i_h, d_h, nb_v, i_v, d_v;

    Alignment();
    Alignment(int nb_h, int i_h, int d_h, int nb_v, int i_v, int d_v);
    sf::Vector2i compute(sf::Vector2i size, sf::Vector2u win_size);
    std::string print();
};

class Button
{
    private:
        sf::Texture sprite_on;
        sf::Texture sprite_hover;
        sf::Texture sprite_off;
        std::string text;
        Alignment alignment;
        sf::RenderWindow* ref_win_p;
        bool defined = false;
        
        bool on = false;
        bool prev_on = false;
        bool hover = false;
        bool prev_hover = false;
    
    public:
        sf::Rect<int> hitbox;

        Button();
        Button(std::string texture_name, std::string text, Alignment, sf::RenderWindow*);
        void reshape();
        bool update();
        void draw(sf::Font);
        bool clicked();

        void set_alignment(Alignment new_align);
};

class Toggle
{
    private:
        bool state;
        Button on_button;
        Button off_button;
        bool defined = false;

    public:
        sf::Rect<int> hitbox;

        Toggle();
        Toggle(Alignment, sf::RenderWindow*, bool);

        void reshape();
        bool update();
        void draw(sf::Font);

        void set_alignment(Alignment new_align);
        bool is_on();
};

#endif //BUTTON_H