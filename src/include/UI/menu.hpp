#ifndef MENU_H
#define MENU_H

#include <map>

#include <SFML/Graphics.hpp>

#include "UI/button.hpp"
#include "save.hpp"

// Functions for Title screen

void draw_title(sf::RenderWindow*, sf::Font);
int title_screen(sf::RenderWindow*, sf::Font);

// Class and Functions for Level selection screen

class LevelGrid
{
    private:
        sf::RenderWindow* win_p;
        Save* save_p;
        std::map<int, Button> levels;
        int nb_pages;
        Button left;
        Button right;
        Button exit_;
    
    public:
        int W, H;
        int page = 0;
        int button_size = 60;
        int delta = 40;

        LevelGrid(sf::RenderWindow*, Save*);
        void reshape();
        bool update();
        void draw(sf::Font);
        int clicked();
};

void draw_levels(sf::RenderWindow*, LevelGrid*, sf::Font);
int level_select(sf::RenderWindow*, Save* save_p, sf::Font);

#endif //BUTTON_H