#ifndef LEVEL_H
#define LEVEL_H

#include <list>
#include <SFML/Graphics.hpp>
#include "grid.hpp"
#include "entities.hpp"
#include "logic.hpp"

class Level
{
    private:
        PlayerLike Player;
        PlayerLike bullet;
        Boxes boxes;
        sf::Vector2i win_tile;
        Grid backGround;
        int nbSteps = 0;
        Logic logic;

        std::map<char, sf::Texture> bg_tiles;

        bool boxPush(Entity* pusher, char direction);//Recursive calls of the pushing function
        void pLikePush(PlayerLike* pushed, char direction);//End call for the pushing function : if a box pushes an enemy
        //(or the player but it's not currently possoble)

        void displayBG(sf::RenderWindow * windowP, sf::Font font);

    public:
        bool won = false;

        Level();
        Level(int number);

        bool isWallForMost(sf::Vector2i coords);    //"Most" being the player and boxes
        bool isWallForBullet(sf::Vector2i coords);
        bool push(char direction);  //First call of the pushing recursive function (the player pushes)
        bool swap(std::string* act);
        bool wait();
        void undo(std::list<std::string>* steps);
        void step(bool didSwap);

        void resize_bg(sf::RenderWindow* windowP);
        void display(sf::RenderWindow* windowP, sf::Font font);
        void animate(sf::RenderWindow* windowP, sf::Font font);
};

#endif //LEVEL_H