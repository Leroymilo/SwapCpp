#ifndef GRID_H
#define GRID_H

#include <SFML/Graphics.hpp>
#include <json/value.h>

#include "globals.hpp"

class Grid
{
    private:
        int h, w;
        std::vector<std::vector<char>> tiles = {{'X'}};
        int x0, y0;
        int pxl_h, pxl_w;
        sf::RenderTexture pre_render;

    public:
        int delta = DELTA;

        Grid();
        Grid(const Grid& tocopy);
        Grid& operator=(const Grid& other);
        ~Grid();
        sf::Vector2i create(Json::Value, std::map<char, sf::Texture>);

        char getTile(sf::Vector2i coords);
        void resize(sf::Vector2f win_size, int Y0);
        void display(sf::RenderWindow* windowPoint);
        void getDisplay(sf::Vector2i* C0, int* deltaP);
};

#endif //GRID_H