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

    bg_tiles['X'].loadFromFile("assets/Tiles/Wall.png");
    bg_tiles['x'].loadFromFile("assets/Tiles/Grate.png");
    bg_tiles['.'].loadFromFile("assets/Tiles/Floor.png");
    bg_tiles['W'].loadFromFile("assets/Tiles/Win.png");

    win_tile = backGround.readFile("levels/level" + str + "/bg.json");
    Player = PlayerLike("levels/level" + str + "/entities.json", "player");
    Player.is_alive = true;
    bullet = PlayerLike("levels/level" + str + "/entities.json", "bullet");
    bullet.is_alive = false;
    boxes = Boxes("levels/level" + str + "/entities.json");
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
    boxes.get_box(coords, &hasBox);
    return hasBox || logic.isWallForBullet(coords);
}

bool Level::push(char direction)
{
    if (!Player.is_alive)
        return false;

    Player.prev_is_alive = true;

    if (bullet.is_alive)
        bullet.prev_is_alive = true;

    bool STEP = !(Player.dir == direction);
    Player.dir = direction;

    sf::Vector2i newCoords = Player.get_next_pos();

    //Checking if there's a wall blocking :
    bool isBlocked = isWallForMost(newCoords);

    //Checking if there's a box blocking :
    if (!isBlocked)
    {
        bool boxBlocked;
        Entity* boxP = boxes.get_box(newCoords, &boxBlocked);
        if (boxBlocked)
            isBlocked = boxPush(boxP, direction);
    }

    //Updating the coords if nothing is blocking:
    if (!isBlocked)
    {
        Player.C = newCoords;
        if (newCoords == win_tile)
            won = true;
        return true;
    }
    return STEP;
}

bool Level::boxPush(Entity* pusher, char direction)
{
    sf::Vector2i newCoords = pusher->get_next_pos(direction);
    //Checking if there's a wall blocking :
    bool isBlocked = isWallForMost(newCoords);

    //Checking if there's a box blocking :
    if (!isBlocked)
    {
        bool boxBlocked;
        Entity* boxP = boxes.get_box(newCoords, &boxBlocked);
        if (boxBlocked)
            isBlocked = boxPush(boxP, direction);
    }

    //Pushing a PlayerLike if it's in front :
    if (bullet.is_alive && bullet.C == newCoords)
        pLikePush(&bullet, direction);

    //Updating the coords if nothing is blocking:
    if (!isBlocked)
        pusher->C = newCoords;
    return isBlocked;
}

void Level::pLikePush(PlayerLike* pushed, char direction)
{
    sf::Vector2i newCoords = pushed->get_next_pos(direction);
    pushed->C = newCoords;
    pushed->dir = direction;
    //It's pushed anyway.
}

bool Level::swap(std::string* act)
{
    if (Player.is_alive && !bullet.is_alive)
    {
        if (isWallForBullet(Player.get_next_pos()))
            return false;
        bullet.prev_is_alive = false;
        bullet.is_alive = true;
        bullet.C = Player.get_next_pos();
        bullet.dir = Player.dir;
        *act = "Shot";
    }
    else if (Player.is_alive && bullet.is_alive)
    {
        if (isWallForMost(bullet.C))
            return false;
        sf::Vector2i tempC = Player.C;
        char tempdir = Player.dir;
        Player.C = bullet.C;
        bullet.C = tempC;
        Player.dir = bullet.dir;
        bullet.dir = tempdir;
        *act = "Swap";
    }
    else if (!Player.is_alive && bullet.is_alive)
    {
        if (isWallForMost(bullet.C))
            return false;
        Player.prev_is_alive = false;
        Player.is_alive = true;
        bullet.prev_is_alive = true;
        bullet.is_alive = false;
        Player.C = bullet.C;
        Player.dir = bullet.dir;
        *act = "Swap";
    }
    else 
        return false;
    return true;
}

bool Level::wait()
{
    if (bullet.is_alive)
    {
        bullet.prev_is_alive = true;
        return true;
    }
    return false;
}

void Level::step(bool didSwap)
{
    //Moving bullet :
    if (bullet.is_alive && !didSwap)
    {
        sf::Vector2i newC = bullet.get_next_pos();

        if (isWallForBullet(newC))
        {
            bullet.revert();
            newC = bullet.get_next_pos();
        }

        if (!isWallForBullet(newC))
            bullet.C = newC;
    }

    //Updating logic :
    std::vector<sf::Vector2i> heavy_pos = boxes.get_boxes_pos();
    if (Player.is_alive)
        heavy_pos.push_back(Player.C);

    std::vector<sf::Vector2i> arrow_pos;
    if (bullet.is_alive)
        arrow_pos.push_back(bullet.C);

    
    sf::Clock clock;
    int t0 = clock.getElapsedTime().asMilliseconds();
    std::vector<sf::Vector2i> updated_activators = logic.update_activators(heavy_pos, arrow_pos, didSwap, &bullet.is_alive);
    logic.update(updated_activators);
    std::cout << "Logic processing time : " << clock.getElapsedTime().asMilliseconds()-t0 << "ms" << std::endl;

    //Checking if player and/or bullet get "killed" :
    Player.is_alive = !((bullet.is_alive && Player.C == bullet.C) || isWallForMost(Player.C)) && Player.is_alive;
    bullet.is_alive = !isWallForBullet(bullet.C) && bullet.is_alive;

    nbSteps++;
}

void Level::undo(std::list<std::string>* steps)
{

}


//Display
void Level::resize_bg(sf::RenderWindow * windowP)   //Should get called every time the game's window is resized
{
    backGround.resize(sf::Vector2f(windowP->getSize()), bg_tiles);
}

void Level::displayBG(sf::RenderWindow * windowP, sf::Font font)
{
    backGround.display(windowP);

    sf::Vector2u winSize = windowP->getSize();

    sf::Text stepDisp;
    stepDisp.setFont(font);
    std::stringstream ss;
    ss << "step : " << nbSteps;
    stepDisp.setString(ss.str());
    stepDisp.setPosition((winSize.x-stepDisp.getLocalBounds().width)/2, winSize.y-40);
    windowP->draw(stepDisp);
    //Use this method to display tips and text on the screen
}

void Level::display(sf::RenderWindow * windowP, sf::Font font)
{
    windowP->clear(sf::Color(0, 0, 230));
    displayBG(windowP, font);

    sf::Vector2i C0;
    int delta;
    backGround.getDisplay(&C0, &delta);

    logic.draw(C0, delta, windowP);

    if (Player.is_alive)
        Player.draw(C0, delta, windowP);
    if (bullet.is_alive)
        bullet.draw(C0, delta, windowP);
    
    boxes.draw(C0, delta, windowP);

    windowP->display();
}

void Level::animate(sf::RenderWindow * windowP, sf::Font font)
{
    sf::Clock clock;
    int t1;
    windowP->clear(sf::Color(0, 0, 230));
    displayBG(windowP, font);

    sf::Vector2f windowSize = sf::Vector2f(windowP->getSize());

    sf::Vector2i C0;
    int delta;
    backGround.getDisplay(&C0, &delta);

    for (int i = 1; i <= 3; i++)//All animations are 4 frames long
    {
        t1 = clock.getElapsedTime().asMilliseconds();
        backGround.display(windowP);

        //Animation of logic elements
        logic.anim(C0, delta, windowP, i);

        //Animation of the player
        if (Player.prev_is_alive && Player.is_alive)
            Player.anim(C0, delta, windowP, i);
        else if (Player.prev_is_alive){}//Add destruction animation
        else if (Player.is_alive)
            Player.draw(C0, delta, windowP);

        //Animation of the bullet
        if (bullet.prev_is_alive && bullet.is_alive)
            bullet.anim(C0, delta, windowP, i);
        else if (bullet.prev_is_alive){}//Add destruction animation
        else if (bullet.is_alive)
            bullet.draw(C0, delta, windowP);

        //Animation of the boxes
        boxes.anim(C0, delta, windowP, i);

        windowP->display();
        while (clock.getElapsedTime().asMilliseconds()-t1 < 20) {}
    }
    display(windowP, font);
}