#ifndef MENU_H
#define MENU_H

#include <map>

#include <SFML/Graphics.hpp>

#include "UI/button.hpp"

// Variables and Functions for Title screen
sf::Texture title;
Button start_;
Button exit_;

void draw_title(sf::RenderWindow*, sf::Font);
int title_screen(sf::RenderWindow*, sf::Font);

// Varibales and Functions for Level selection screen
int lvl_grid_w, lvl_grid_h;
int page;
int lvl_button_size = 60, lvl_button_delta = 40;

class LevelGrid
{
    private:
        sf::RenderWindow* win_p;
        std::map<int, Button> levels;
    
    public:
        LevelGrid(sf::RenderWindow*);
        void reshape();
        bool update();
        void draw(sf::Font);
        int clicked();
};

sf::Vector2i lvl_grid_size(sf::RenderWindow* win_p);
void draw_levels(sf::RenderWindow*, sf::Font);
int level_select(sf::RenderWindow*, sf::Font);

#endif //BUTTON_H