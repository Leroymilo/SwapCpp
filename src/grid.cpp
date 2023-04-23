#include "gameplay/grid.hpp"

#include <fstream>
#include <iostream>

Grid::Grid(){}

Grid::Grid(const Grid& tocopy)
{
    this->tiles = tocopy.tiles;
}

Grid& Grid::operator=(const Grid& other)
{
    if (this == &other)
        return *this;

    this->tiles = other.tiles;
    return *this;
}

sf::Vector2i Grid::create(Json::Value json_bg, std::map<char, sf::Texture> textures)
{
    h = json_bg["H"].asInt();
    w = json_bg["W"].asInt();
    int endX = json_bg["EndX"].asInt();
    int endY = json_bg["EndY"].asInt();
    tiles.resize(h);
    
    for (int y=0; y<h; y++){
        tiles[y].resize(w);
        for (int x=0; x<w; x++){
            tiles[y][x] = json_bg["BG"][y][x].asCString()[0];
        }
    }

    tiles[endY][endX] = 'W';

    pre_render.create(w * delta, h * delta);
    pre_render.clear(sf::Color::Transparent);
    sf::RectangleShape tile(sf::Vector2f(delta, delta));

    for (int x=0; x<w; x++){
        for (int y=0; y<h; y++){
            tile.setTexture(&textures[tiles[y][x]]);
            tile.setPosition(delta * x, delta * y);
            pre_render.draw(tile);
        }
    }

    return sf::Vector2i(endX, endY);
}

char Grid::getTile(sf::Vector2i coords)
{
    return tiles[coords.y][coords.x];
}

void Grid::resize(sf::Vector2f win_size, int Y0)
{
    if (win_size.y <= 0)
    {
        delta = 0;
        return;
    }

    float shapeQ = float(w)/(float(h) + 0.5);

    float WinQ = float(win_size.x)/float(win_size.y);

    if (WinQ > shapeQ)
        delta = win_size.y/(h+0.5);
    else
        delta = win_size.x/w;
    
    delta -= delta % 16;
    if (delta == 0) return;

    pxl_w = delta*w, pxl_h = delta*h;
    x0 = (win_size.x-pxl_w)/2, y0 = (win_size.y-pxl_h)/2 + delta/4 + Y0;
}

void Grid::display(sf::RenderWindow* windowPoint, sf::Font font)
{
    if (delta > 0)
    {
        sf::RectangleShape rect(sf::Vector2f(pxl_w, pxl_h));
        rect.setTexture(&pre_render.getTexture());
        rect.setPosition(x0, y0+delta*h);
        rect.setScale(sf::Vector2f(1, -1));
        windowPoint->draw(rect);
    }
    else
    {
        sf::Vector2u winSize = windowPoint->getSize();
        sf::Text helpDisp("smol window owo", font, 12);
        sf::FloatRect bounds = helpDisp.getLocalBounds();
        helpDisp.setPosition((winSize.x-bounds.width-bounds.left)/2, y0);
        windowPoint->draw(helpDisp);
    }
}

void Grid::getDisplay(sf::Vector2i* C0, int* deltaP)
{
    *C0 = sf::Vector2i(x0, y0);
    *deltaP = delta;
}

Grid::~Grid()
{
    //TODO mdr
}