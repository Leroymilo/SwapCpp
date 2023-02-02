#ifndef SETTINGS_H
#define SETTINGS_H

#include "UI/button.hpp"
#include "save.hpp"

class Option_Line
{
    private:
        sf::RenderWindow* ref_win_p;
        std::string text;
        Toggle button;
    
    public:
        Option_Line();
        Option_Line(sf::RenderWindow*, std::string, bool, Alignment);

        void reshape();
        bool update();
        void draw(sf::Font font);
};

class Options
{
    private:
        Save* ref_save_p;
        sf::RenderWindow* ref_win_p;
        std::map<std::string, Option_Line> lines;

    public:
        Options();
        Options(Save*, sf::RenderWindow*, sf::Font);

        void reshape();
        bool update();
        void draw(sf::Font font);
};

#endif //SETTINGS_H