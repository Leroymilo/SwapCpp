#ifndef SETTINGS_H
#define SETTINGS_H

#include "UI/button.hpp"
#include "save.hpp"

class Option_Line
{
    private:
        sf::RenderWindow* ref_win_p;
        Alignment text_align;
        std::string text;
        CycleButton button;
    
    public:
        Option_Line();
        Option_Line(sf::RenderWindow*, std::string key_word, std::string text, bool, Alignment);

        bool get_button_state();

        void reshape();
        bool update();
        void draw();
};

class Options
{
    private:
        Save* ref_save_p;
        sf::RenderWindow* ref_win_p;
        std::map<std::string, Option_Line> lines;

    public:
        Button apply;
        Button exit_;

        Options();
        Options(Save*, sf::RenderWindow*);

        void reshape();
        bool update();
        void draw();
};

int settings(sf::RenderWindow*, Save*);

#endif //SETTINGS_H