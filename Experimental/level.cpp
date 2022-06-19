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
    logic = Logic("levels/level" + str + "/logic.json");
}


//Gameplay
bool Level::isWallForMost(sf::Vector2i coords)
{
    if (backGround.getTile(coords) == 'X' or backGround.getTile(coords) == 'x')
        return true;
    return logic.isWallForMost(coords);
}

bool Level::isWallForBullet(sf::Vector2i coords)
{
    if (backGround.getTile(coords) == 'X')
        return true;
    bool hasBox;
    boxes.getBox(coords, &hasBox);
    return hasBox || logic.isWallForBullet(coords);
}

bool Level::push(char direction)
{
    bool STEP = !(Player.direction == direction);

    Player.direction = direction;
    sf::Vector2i newCoords = Player.getNextPos();

    //Checking if there's a wall blocking :
    bool isBlocked = isWallForMost(newCoords);

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
    bool isBlocked = isWallForMost(newCoords);

    //Checking if there's a box blocking :
    if (!isBlocked)
    {
        bool boxBlocked;
        Entity* boxP = boxes.getBox(newCoords, &boxBlocked);
        if (boxBlocked)
            isBlocked = boxPush(boxP, direction);
    }

    //Pushing a PlayerLike if it's in front :
    if (balive && bullet.C == newCoords)
        pLikePush(&bullet, direction);

    //Updating the coords if nothing is blocking:
    if (!isBlocked)
        pusher->C = newCoords;
    return isBlocked;
}

void Level::pLikePush(PlayerLike* pushed, char direction)
{
    sf::Vector2i newCoords = pushed->getNextPos(direction);
    pushed->C = newCoords;
    pushed->direction = direction;
    //It's pushed anyway.
    //It will be destroyed if it's inside another object :

    balive = !isWallForBullet(newCoords);
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

bool Level::wait()
{
    if (balive)
        return true;
    return false;
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

    //Updating logic :
    std::vector<sf::Vector2i> heavy_pos = boxes.get_boxes_pos();
    if (Palive)
        heavy_pos.push_back(Player.C);

    std::vector<sf::Vector2i> arrow_pos;
    if (balive)
        arrow_pos.push_back(bullet.C);

    std::vector<sf::Vector2i> updated_activators = logic.update_activators(heavy_pos, arrow_pos, didSwap, &balive);
    logic.update(updated_activators);

    nbSteps++;
}


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

    logic.draw(C0, delta, windowP);

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

    for (int i = 1; i <= 4; i++)//All animations are 4 frames long
    {
        windowP->draw(BG);

        //Animation of logic elements
        logic.anim(prevStep.logic, C0, delta, windowP, i);

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