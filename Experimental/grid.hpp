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
        Grid(const Grid& tocopy);
        Grid& operator=(const Grid& other);
        ~Grid();
        sf::Vector2i readFile(std::string directory);
        void setGrid(char ** grid, int w, int h);

        char getTile(sf::Vector2i coords);
        void display(sf::RenderWindow* windowPoint, std::map<char, sf::Texture> textures);
        void getDisplay(sf::Vector2i* C0, int* deltaP);
};

#endif //GRID_H
