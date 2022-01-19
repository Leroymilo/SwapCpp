#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics.hpp>
#include "grid.hpp"
#include "entities.hpp"

class Level
{
    private:
        PlayerLike Player;
        bool Palive = true;
        PlayerLike bullet;
        bool balive = false;
        Boxes boxes;
        Grid backGround;
        int nbSteps;
        Level * prevLevel;

        bool boxPush(Entity* pusher, char direction);//Recursive calls of the pushing function
        bool pLikePush(PlayerLike* pusher, char direction);//End call for the pushing function : if a box pushes an enemy

    public:
        Level();
        Level(int number);

        bool isWallForPlayer(sf::Vector2i coords);
        bool isWallForBullet(sf::Vector2i coords);
        bool isWallForBox(sf::Vector2i coords);
        bool push(char direction);  //First call of the pushing recursive function (the player pushes)
        bool swap();
        void step(bool didSwap);
        void undo();
        void display(sf::RenderWindow * windowP);
};

#endif //LEVEL_H