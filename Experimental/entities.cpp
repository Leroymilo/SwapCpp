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

    C = sf::Vector2i(actualJson[name]["X"].asInt(), actualJson[name]["Y"].asInt());
    direction = actualJson[name]["dir"].asCString()[0];

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

void PlayerLike::revert()
{
    if (direction == 'U')
        direction = 'D';
    else if (direction == 'R')
        direction = 'L';
    else if (direction == 'D')
        direction = 'U';
    else if (direction == 'L')
        direction = 'R';
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

Boxes::Boxes(const Boxes& tocopy)
{
    this->setLists(tocopy.nbBoxes, tocopy.list, tocopy.listAlive);
    std::cout << "Boxes copied !" << std::endl;
}

Boxes& Boxes::operator=(const Boxes& other)
{
    this->setLists(other.nbBoxes, other.list, other.listAlive);
    return *this;
}

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
        sf::Vector2i C(actualJson["Boxes"][i]["X"].asInt(), actualJson["Boxes"][i]["Y"].asInt());
        list[i] = Entity(C, sprite);
        listAlive[i] = true;
    }
}

void Boxes::setLists(int n, Entity * list, bool * listAlive)
{
    this->nbBoxes = n;
    this->list = new Entity [nbBoxes];
    this->listAlive = new bool [nbBoxes];
    sf::Texture sprite;
    sprite.loadFromFile("assets/Entities/Box.png");

    for (int i=0; i<nbBoxes; i++)
    {
        sf::Vector2i C(list[i].C.x, list[i].C.y);
        this->list[i] = Entity(C, sprite);
        this->listAlive[i] = listAlive[i];
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

Boxes::~Boxes()
{
    delete[] this->list;
    delete[] this->listAlive;
}