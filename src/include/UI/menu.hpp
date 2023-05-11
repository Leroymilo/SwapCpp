#ifndef MENU_H
#define MENU_H

#include <list>
#include <map>

#include <SFML/Graphics.hpp>

#include "UI/button.hpp"
#include "save.hpp"

// Class for Title screen

class Title
{
    private:
        sf::RenderWindow *ref_win_p;
        sf::Color bg_color = sf::Color(138, 208, 234);
        sf::Vector2f full_title_size;
        float scale = 1;

        sf::Texture title_texture;
        sf::RectangleShape title_sprite;
        sf::Vector2f title_pos;

        sf::Texture button_texture;
        sf::Image button_image;
        Button start_, exit_, settings;
        
        void reshape();
        void draw();
    
    public:

        Title(sf::RenderWindow*);
        int run();
};

// Class and Functions for Level selection screen

class LevelSelect
{
    private:
        sf::RenderWindow* ref_win_p;
        sf::Color bg_color = sf::Color(20, 30, 120);
        sf::Vector2f full_grid_size;
        float scale = 1;

        Save* save_p;

        sf::Texture button_texture;
        sf::Image button_image;
        sf::Texture exit_texture;
        sf::Image exit_image;

        int font_size;
        std::map<int, Button> levels;
        std::list<int> current_levels;
        Button left;
        Button right;
        Button exit_;

        int nb_pages;
        int page;

        void change_page(int);
    
    public:
        LevelSelect(sf::RenderWindow*, Save*, int);
        void reshape();
        bool update();
        void draw();
        int clicked();

        int run();
};

// void draw_levels(sf::RenderWindow*, LevelGrid*);
// int level_select(sf::RenderWindow*, Save*, int);

#endif //MENU_H