#ifndef GRID_H
#define GRID_H

#include <SFML/Graphics.hpp>

class Grid
{
    private:
        int h, w;
        char ** tiles;
        int x0 = 0, y0 = 0;

    public:
        int delta = 16;

        Grid();
        Grid(const int h, const int w, char defChar);
        Grid(std::string directory);

        char getTile(sf::Vector2i coords);

        void display(sf::RenderWindow* windowPoint);
        void getDisplay(sf::Vector2i* C0, int* deltaP);
};

#endif //GRID_H
