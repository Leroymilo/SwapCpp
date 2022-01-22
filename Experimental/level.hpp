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
        int nbSteps = 0;

        bool boxPush(Entity* pusher, char direction);//Recursive calls of the pushing function
        bool pLikePush(PlayerLike* pusher, char direction);//End call for the pushing function : if a box pushes an enemy

        void displayBG(sf::RenderWindow * windowP, sf::Font font);

    public:
        Level();
        Level(int number);

        bool isWallForPlayer(sf::Vector2i coords);
        bool isWallForBullet(sf::Vector2i coords);
        bool isWallForBox(sf::Vector2i coords);
        bool push(char direction);  //First call of the pushing recursive function (the player pushes)
        bool swap();
        bool wait();
        void step(bool didSwap);
        void display(sf::RenderWindow * windowP, sf::Font font);
        void animate(sf::RenderWindow * windowP, sf::Font font, Level prevStep);
};

#endif //LEVEL_H