#include "level.hpp"

#include <json/value.h>
#include <json/json.h>

#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>


//Constructor and meta
Level::Level(){}

Level::Level(int number)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(3) << number;
    std::string str = ss.str();

    backGround.readFile("levels/level" + str + "/bg.json");
    Player = PlayerLike("levels/level" + str + "/entities.json", "player");
    bullet = PlayerLike("levels/level" + str + "/entities.json", "bullet");
    boxes.readFile("levels/level" + str + "/entities.json");
}

//Gameplay
bool Level::isWallForPlayer(sf::Vector2i coords)
{
    if (backGround.getTile(coords) == 'X' or backGround.getTile(coords) == 'x' or backGround.getTile(coords) == 'T')
        return true;
    return false;
}

bool Level::isWallForBox(sf::Vector2i coords)
{
    if (backGround.getTile(coords) == 'X' or backGround.getTile(coords) == 'x' or backGround.getTile(coords) == 'T')
        return true;
    return false;
}

bool Level::isWallForBullet(sf::Vector2i coords)
{
    if (backGround.getTile(coords) == 'X')
        return true;
    bool hasBox;
    boxes.getBox(coords, &hasBox);
    return hasBox;
}

bool Level::push(char direction)
{
    bool STEP = !(Player.direction == direction);

    Player.direction = direction;
    sf::Vector2i newCoords = Player.getNextPos();

    //Checking if there's a wall blocking :
    bool isBlocked = isWallForPlayer(newCoords);

    //Checking if there's a box blocking :
    if (!isBlocked)
    {
        bool boxBlocked;
        Entity* boxP = boxes.getBox(newCoords, &boxBlocked);
        if (boxBlocked)
            isBlocked = boxPush(boxP, direction);
    }

    //Updating the coords if nothing is blocking:
    if (!isBlocked)
    {
        Player.C = newCoords;
        return true;
    }
    return STEP;
}

bool Level::boxPush(Entity* pusher, char direction)
{
    sf::Vector2i newCoords = pusher->getNextPos(direction);
    //Checking if there's a wall blocking :
    bool isBlocked = isWallForBox(newCoords);

    //Checking if there's a box blocking :
    if (!isBlocked)
    {
        bool boxBlocked;
        Entity* boxP = boxes.getBox(newCoords, &boxBlocked);
        if (boxBlocked)
            isBlocked = boxPush(boxP, direction);
    }

    //Updating the coords if nothing is blocking:
    if (!isBlocked)
        pusher->C = newCoords;
    return isBlocked;
}

bool Level::swap()
{
    if (Palive && !balive)
    {
        balive = true;
        bullet.C = Player.getNextPos();
        bullet.direction = Player.direction;
    }
    else if (Palive && balive)
    {
        sf::Vector2i tempC = Player.C;
        char tempdir = Player.direction;
        Player.C = bullet.C;
        bullet.C = tempC;
        Player.direction = bullet.direction;
        bullet.direction = tempdir;
    }
    else if (!Palive && balive)
    {
        Palive = true;
        balive = false;
        Player.C = bullet.C;
        Player.direction = bullet.direction;
    }
    else 
        return false;
    return true;
}

void Level::step(bool didSwap)
{
    if (balive && !didSwap)
    {
        sf::Vector2i newC = bullet.getNextPos();

        if (isWallForBullet(newC))
        {
            bullet.revert();
            newC = bullet.getNextPos();
        }

        if (!isWallForBullet(newC))
            bullet.C = newC;
    }
    nbSteps++;
}


//Logic


//Display
void Level::displayBG(sf::RenderWindow * windowP, sf::Font font)
{
    sf::Vector2u winSize = windowP->getSize();
    
    windowP->clear(sf::Color(0, 0, 120));
    backGround.display(windowP);

    sf::Text stepDisp;
    stepDisp.setFont(font);
    std::stringstream ss;
    ss << "step : " << nbSteps;
    stepDisp.setString(ss.str());
    stepDisp.setPosition((winSize.x-stepDisp.getLocalBounds().width)/2, winSize.y-stepDisp.getLocalBounds().height-10);
    windowP->draw(stepDisp);
    //Use this method to display tips and text on the screen
}

void Level::display(sf::RenderWindow * windowP, sf::Font font)
{
    displayBG(windowP, font);

    sf::Vector2i C0;
    int delta;
    backGround.getDisplay(&C0, &delta);
    if (Palive)
        Player.draw(C0, delta, windowP);
    if (balive)
        bullet.draw(C0, delta, windowP);
    
    boxes.draw(C0, delta, windowP);

    windowP->display();
}

void Level::animate(sf::RenderWindow * windowP, sf::Font font, Level prevStep)
{
    sf::Vector2f windowSize = sf::Vector2f(windowP->getSize());

    displayBG(windowP, font);

    sf::Vector2i C0;
    int delta;
    backGround.getDisplay(&C0, &delta);
    sf::RectangleShape BG(windowSize);
    sf::Texture texture;
    texture.create(windowSize.x, windowSize.y);
    texture.update(*windowP);
    BG.setTexture(&texture);

    for (int i = 0; i <= 4; i++)//All animations are 4 frames long
    {
        windowP->draw(BG);
        //Animation of the player
        
        if (prevStep.Palive && Palive)
            Player.anim(prevStep.Player.C, C0, delta, windowP, i);
        else if (prevStep.Palive){}
        else if (Palive)
            Player.draw(C0, delta, windowP);

        //Animation of the bullet
        if (prevStep.balive && balive)
            bullet.anim(prevStep.bullet.C, C0, delta, windowP, i);
        else if (prevStep.balive){}
        else if (balive)
            bullet.draw(C0, delta, windowP);

        //Animation of the boxes
        boxes.anim(prevStep.boxes, C0, delta, windowP, i);

        windowP->display();
        sf::sleep(sf::milliseconds(10));
    }
}