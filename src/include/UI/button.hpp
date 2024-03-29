#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>

#include "globals.hpp"

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
        sf::Texture texture;
        sf::Sprite sprite;
        std::string text;
        Alignment alignment;
        sf::RenderWindow* ref_win_p;
        
        int state = 0;
        int prev_state = 0;

        bool on = false;
        bool prev_on = false;
        bool hover = false;
        bool prev_hover = false;
    
    public:
        bool defined = false;
        sf::Rect<int> hitbox;
        sf::Vector2i shape;
        unsigned int font_size = 30;

        Button();
        Button(std::string texture_name, std::string text, Alignment, sf::RenderWindow*);
        void reshape();
        bool update();
        void draw();
        bool clicked();

        void set_alignment(Alignment new_align);
};

class CycleButton
{
    private:
        int state;
        int nb_buttons;
        std::vector<Button> buttons;
        bool defined = false;

    public:
        sf::Rect<int> hitbox;

        CycleButton();
        CycleButton(std::string key_word, Alignment, sf::RenderWindow*, int);

        void reshape();
        bool update();
        void draw();

        void set_alignment(Alignment new_align);
        int get_state();
};

#endif //BUTTON_H