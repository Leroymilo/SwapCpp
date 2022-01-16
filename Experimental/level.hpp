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

        bool boxPush(Entity* pusher, char direction);//Recursive calls of the pushing function
        bool pLikePush(PlayerLike* pusher, char direction);  
        //End call for the pushing function : if a box pushes a monster or the bullet

    public:
        Level(int number);
        Level copy();

        void input(char action);
        bool isWallForPlayer(sf::Vector2i coords);
        bool isWallForBullet(sf::Vector2i coords);
        bool isWallForBox(sf::Vector2i coords);
        void push(char direction);  //First call of the pushing recursive function (the player pushes)
        void swap();
        void step();
        void display(sf::RenderWindow * windowP);
};

#endif //LEVEL_H