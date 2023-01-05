#ifndef LEVEL_H
#define LEVEL_H

#include <list>

#include <SFML/Graphics.hpp>

#include "gameplay/grid.hpp"
#include "gameplay/entities.hpp"
#include "gameplay/logic.hpp"
#include "UI/button.hpp"


class Level
{
    private:
        PlayerLike Player;
        PlayerLike bullet;
        Boxes boxes;
        sf::Vector2i win_tile;
        Grid backGround;
        Logic logic;

        std::map<char, sf::Texture> bg_tiles;

        bool boxPush(Entity* pusher, char direction, int* nb_pushed);//Recursive calls of the pushing function
        void pLikePush(PlayerLike* pushed, char direction);//End call for the pushing function : if a box pushes an enemy
        //(or the player but it's not currently possoble)

        void displayBG(sf::RenderWindow * windowP, sf::Font font);

    public:
        int nbSteps = 0;
        bool won = false;

        Level();
        Level(int number);
        std::string get_pLike_state();  //To record the states of the Player and bullet into the list of steps

        bool isWallForMost(sf::Vector2i coords);    //"Most" being the player and boxes
        bool isWallForBullet(sf::Vector2i coords);
        bool push(char direction, std::string* act);  //First call of the pushing recursive function (the player pushes)
        bool swap(std::string* act);
        bool wait();
        void undo(std::list<std::string>* steps);
        void step(bool didSwap);
        void step_end_logic();

        void resize_bg(sf::RenderWindow* windowP);
        void display(sf::RenderWindow* windowP, sf::Font font, bool disp = true);
        void animate(sf::RenderWindow* windowP, sf::Font font);
};

void display_pause(sf::RenderWindow*, sf::Font);    // Function to display the pause menu over the level
int pause(Level*, sf::RenderWindow*, sf::Font);     // Function to handle pause menu interractions
int run(int, sf::RenderWindow*, sf::Font, int*);          // Function replacing the original main loop of swap.cpp

#endif //LEVEL_H