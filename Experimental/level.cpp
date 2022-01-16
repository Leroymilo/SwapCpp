#include "level.hpp"

#include <json/value.h>
#include <json/json.h>

#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>

//Constructor and meta
Level::Level(int number)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(3) << number;
    std::string str = ss.str();

    backGround = Grid("levels/level" + str + "/bg.json");
    Player = PlayerLike("levels/level" + str + "/entities.json", "player");
    boxes = Boxes("levels/level" + str + "/entities.json");
}


//Gameplay
void Level::input(char action)
{
    char directions [] = {'U', 'R', 'D', 'L'};
    char* end = directions+4;
    if (std::find(directions, end, action) != end)
        push(action);
    else if (action == ' ')
        swap();
}

bool Level::isWallForPlayer(sf::Vector2i coords)
{
    if (backGround.getTile(coords) == 'X' or backGround.getTile(coords) == 'x' or backGround.getTile(coords) == 'T')
        return true;
    return false;
}

void Level::push(char direction)
{
    Player.direction = direction;
    sf::Vector2i newCoords = Player.getNextPos();
    //Checking if there's a wall blocking :
    bool isBlocked = isWallForPlayer(newCoords);

    //Checking if there's a box blocking :
    if (!isBlocked)
    {
        bool boxBlocked;
        Entity box = boxes.getBox(newCoords, &boxBlocked);
        if (boxBlocked)
            isBlocked = boxPush(box, direction);
    }

    //Updating the coords if nothing is blocking:
    if (!isBlocked)
        Player.C = newCoords;
}

bool Level::boxPush(Entity pusher, char direction)
{
    sf::Vector2i newCoords = pusher.getNextPos(direction);
    //Checking if there's a wall blocking :
    bool isBlocked = isWallForPlayer(newCoords);

    //Checking if there's a box blocking :
    if (!isBlocked)
    {
        bool boxBlocked;
        Entity box = boxes.getBox(newCoords, &boxBlocked);
        if (boxBlocked)
            isBlocked = boxPush(box, direction);
    }

    //Updating the coords if nothing is blocking:
    if (!isBlocked)
        pusher.C = newCoords;
    return isBlocked;
}

void Level::swap()
{
    
}


//Logic


//Display
void Level::display(sf::RenderWindow * windowP)
{
    backGround.display(windowP);

    sf::Vector2i C0;
    int delta;
    backGround.getDisplay(&C0, &delta);
    Player.draw(C0, delta, windowP);
    boxes.draw(C0, delta, windowP);

    windowP->display();
}