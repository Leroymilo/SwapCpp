#ifndef GRID_H
#define GRID_H

#include <SFML/Graphics.hpp>

class Grid
{
    private:
        int h, w;
        std::vector<std::vector<char>> tiles = {{'X'}};
        int x0, y0;
        sf::RenderTexture pre_render;

    public:
        int delta = 16;

        Grid();
        Grid(const Grid& tocopy);
        Grid& operator=(const Grid& other);
        ~Grid();
        sf::Vector2i readFile(std::string directory);

        char getTile(sf::Vector2i coords);
        void resize(sf::Vector2f win_size, std::map<char, sf::Texture> textures);
        void display(sf::RenderWindow* windowPoint);
        void getDisplay(sf::Vector2i* C0, int* deltaP);
};

#endif //GRID_H