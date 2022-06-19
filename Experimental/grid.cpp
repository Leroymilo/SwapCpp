#include "grid.hpp"

#include <json/value.h>
#include <json/json.h>

#include <fstream>
#include <iostream>

Grid::Grid(){}

Grid::Grid(const Grid& tocopy)
{
    this->setGrid(tocopy.tiles, tocopy.w, tocopy.h);
}

Grid& Grid::operator=(const Grid& other)
{
    if (this == &other)
        return *this;

    this->setGrid(other.tiles, other.w, other.h);
    return *this;
}

sf::Vector2i Grid::readFile(std::string directory)
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

    return sf::Vector2i(endX, endY);
}

void Grid::setGrid(char ** newtiles, int w, int h)
{
    this->w = w;
    this->h = h;
    tiles = new char * [h];
    char * tileLine;
    for (int y=0; y<h; y++){
        tileLine = new char [w];
        for (int x=0; x<w; x++){
            tileLine[x] = newtiles[y][x];
        }
        tiles[y] = tileLine;
    }
}

char Grid::getTile(sf::Vector2i coords)
{
    return tiles[coords.y][coords.x];
}

void Grid::display(sf::RenderWindow* windowPoint, std::map<char, sf::Texture> textures)
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
            tile.setTexture(&textures[tiles[y][x]]);
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