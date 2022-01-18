#include "grid.hpp"

#include <json/value.h>
#include <json/json.h>

#include <fstream>
#include <iostream>

Grid::Grid(){}

void Grid::readFile(std::string directory)
{
    std::ifstream file(directory);
    Json::Value actualJson;
    Json::Reader reader;

    reader.parse(file, actualJson);

    h = actualJson["H"].asInt();
    w = actualJson["W"].asInt();
    int endX = actualJson["EndX"].asInt();
    int endY = actualJson["EndY"].asInt();
    tiles = new char * [h];
    char * tileLine;

    for (int y=0; y<h; y++){
        tileLine = new char [w];
        for (int x=0; x<w; x++){
            if (x==endX and y==endY)
                tileLine[x] = 'W';
            else
                tileLine[x] = actualJson["BG"][y][x].asCString()[0];
        }
        tiles[y] = tileLine;
    }
}

char Grid::getTile(sf::Vector2i coords)
{
    return tiles[coords.y][coords.x];
}

void Grid::display(sf::RenderWindow* windowPoint)
{
    float shapeQ = float(w)/float(h);

    sf::Vector2i WinSize(windowPoint->getSize());
    float WinQ = float(WinSize.x)/float(WinSize.y);

    if (WinQ > shapeQ)
        delta = WinSize.y/h;
    else
        delta = WinSize.x/w;
    
    delta -= delta % 16;
    float pxlW = delta*w, pxlH = delta*h;

    x0 = (WinSize.x-pxlW)/2, y0 = (WinSize.y-pxlH)/2;

    sf::RectangleShape tile(sf::Vector2f(delta, delta));

    for (int x=0; x<w; x++){
        for (int y=0; y<h; y++){
            char color = tiles[y][x];
            if (color == 'X')
                tile.setFillColor(sf::Color::Black);
            else if (color == '.')
                tile.setFillColor(sf::Color::White);
            else if (color == 'W')
                tile.setFillColor(sf::Color::Green);
            tile.setPosition(x0+delta*x, y0+delta*y);
            windowPoint->draw(tile);
        }
    }
}

void Grid::getDisplay(sf::Vector2i* C0, int* deltaP)
{
    *C0 = sf::Vector2i(x0, y0);
    *deltaP = delta;
}

Grid::~Grid()
{
    for(int i = 0; i < h; i++)
        delete[] this->tiles[i];
    delete[] this->tiles;
}