#include "entities.hpp"

#include <json/value.h>
#include <json/json.h>

#include <fstream>
#include <iostream>

//Methods for Entity
Entity::Entity(){}


Entity::Entity(sf::Vector2i C, sf::Texture sprite)
{
    this->C = C;
    this->sprite = sprite;
}

sf::Vector2i Entity::getNextPos(char direction)
{
    if (direction == 'U')
        return sf::Vector2i(C.x, C.y-1);
    else if (direction == 'R')
        return sf::Vector2i(C.x+1, C.y);
    else if (direction == 'D')
        return sf::Vector2i(C.x, C.y+1);
    else if (direction == 'L')
        return sf::Vector2i(C.x-1, C.y);
}

void Entity::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint)
{
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    tile.setTexture(&sprite);
    tile.setPosition(C0.x+delta*C.x, C0.y+delta*C.y);
    windowPoint->draw(tile);
}


//Methods for PlayerLike
PlayerLike::PlayerLike(){}

PlayerLike::PlayerLike(std::string directory, std::string name)
{
    std::ifstream file(directory);
    Json::Value actualJson;
    Json::Reader reader;

    reader.parse(file, actualJson);

    C = sf::Vector2i(actualJson["PlayerX"].asInt(), actualJson["PlayerY"].asInt());
    direction = 'U';

    char directions [] = {'U', 'R', 'D', 'L'};
    for (int i=0; i<4; i++)
    {
        sprite.loadFromFile("assets/Entities/" + name + directions[i] + ".png");
        sprites[directions[i]] = sprite;
    }
    sprite = sprites[direction];
}

sf::Vector2i PlayerLike::getNextPos()
{
    if (direction == 'U')
        return sf::Vector2i(C.x, C.y-1);
    else if (direction == 'R')
        return sf::Vector2i(C.x+1, C.y);
    else if (direction == 'D')
        return sf::Vector2i(C.x, C.y+1);
    else if (direction == 'L')
        return sf::Vector2i(C.x-1, C.y);
}

void PlayerLike::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint)
{
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    tile.setTexture(&sprites[direction]);
    tile.setPosition(C0.x+delta*C.x, C0.y+delta*C.y);
    windowPoint->draw(tile);
}


//Methods for Boxes
Boxes::Boxes(){}

void Boxes::readFile(std::string directory)
{
    std::ifstream file(directory);
    Json::Value actualJson;
    Json::Reader reader;

    reader.parse(file, actualJson);

    nbBoxes = actualJson["nbBoxes"].asInt();
    list = new Entity [nbBoxes];
    listAlive = new bool [nbBoxes];
    sf::Texture sprite;
    sprite.loadFromFile("assets/Entities/Box.png");

    for (int i=0; i<nbBoxes; i++)
    {
        sf::Vector2i C(actualJson["Boxes"][i]["BoxX"].asInt(), actualJson["Boxes"][i]["BoxY"].asInt());
        list[i] = Entity(C, sprite);
        listAlive[i] = true;
    }
}

Entity* Boxes::getBox(sf::Vector2i coords, bool* hasBox)
{
    for (int iBox=0; iBox<nbBoxes; iBox++)
    {
        if (listAlive[iBox] && list[iBox].C == coords)
        {
            *hasBox = true;
            return &list[iBox];
        }
    }
    *hasBox = false;
    return &list[0];
}

void Boxes::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint)
{
    for (int iBox=0; iBox<nbBoxes; iBox++)
    {
        if (listAlive[iBox])
            list[iBox].draw(C0, delta, windowPoint);
    }
}