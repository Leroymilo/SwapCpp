#ifndef SETTINGS_H
#define SETTINGS_H

#include "UI/button.hpp"
#include "save.hpp"

class Options
{
    private:
        Save* ref_save_p;
        sf::RenderWindow* ref_win_p;

    public:
        Options();
        Options(Save*, sf::RenderWindow*);

        void reshape();
        bool update();
        void draw(sf::Font font);
};

#endif //SETTINGS_H