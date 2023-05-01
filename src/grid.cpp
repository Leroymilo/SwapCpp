#include <fstream>
#include <iostream>

#include "gameplay/grid.hpp"

sf::Vector2i neighbors[] = {
    sf::Vector2i(0, -1),
    sf::Vector2i(1, 0),
    sf::Vector2i(0, 1),
    sf::Vector2i(-1, 0)
};

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
    tiles.resize(h);

    int endX, endY;
    
    for (int y=0; y<h; y++){
        tiles[y].resize(w);
        for (int x=0; x<w; x++){
            tiles[y][x] = json_bg["BG"][y].asCString()[x];
            if (tiles[y][x] == 'E') {
                endX = x, endY = y;
            }
        }
    }

    // Initializing wall sprite :
    sf::Sprite wall_sprite(textures['X'], sf::IntRect(0, 0, DELTA, DELTA));

    // Initializing other sprites :
    std::map<char, sf::Sprite> sprites;
    sprites['T'].setTexture(textures['T'], true);
    sprites['E'].setTexture(textures['E'], true);

    // Fill background with floor :
    pre_render.create(w * delta, h * delta);
    pre_render.clear(sf::Color::Transparent);
    textures['.'].setRepeated(true);
    sf::Sprite floor_sprite(textures['.'], sf::IntRect(0, 0, DELTA * w, DELTA * h));
    pre_render.draw(floor_sprite);

    for (int x=0; x<w; x++){
        for (int y=0; y<h; y++){
            char tile_type = tiles[y][x];

            // Wall orientation :
            if (tile_type == 'X') {
                int value = 0;
                for (int i = 0; i < 4; i++) {
                    int nx = x + neighbors[i].x, ny = y + neighbors[i].y;
                    if (0 <= nx && nx < w && 0 <= ny && ny < h && tiles[ny][nx] == 'X') {
                        value += (1 << i);
                    }
                }
                int left = (value%4), top = (value/4);
                wall_sprite.setTextureRect(sf::IntRect(left * DELTA, top * DELTA, DELTA, DELTA));
                wall_sprite.setPosition(DELTA * x, DELTA * y);
                pre_render.draw(wall_sprite);
            }

            // Thorns or End :
            else if (tile_type != '.') {
                sprites[tile_type].setPosition(DELTA * x, DELTA * y);
                pre_render.draw(sprites[tile_type]);
            }
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

    float shapeQ = float(w)/(float(h) + 1);

    float WinQ = float(win_size.x)/float(win_size.y);

    if (WinQ > shapeQ)
        delta = win_size.y/(h + 1);
    else
        delta = win_size.x/w;
    
    delta -= delta % DELTA;
    if (delta == 0) return;

    pxl_w = delta*w, pxl_h = delta*h;
    x0 = (win_size.x-pxl_w)/2, y0 = (win_size.y-pxl_h)/2 + delta/4 + Y0;
}

void Grid::display(sf::RenderWindow* windowPoint)
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
        sf::Text helpDisp("smol window owo", font.get_font(), 12);
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