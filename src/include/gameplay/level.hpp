#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <vector>
#include <list>

#include <SFML/Graphics.hpp>

#include "gameplay/grid.hpp"
#include "gameplay/entities.hpp"
#include "gameplay/logic.hpp"
#include "UI/button.hpp"

std::string make_level_name(int nb);

class Level
{
    private:
        PlayerLike Player;
        PlayerLike ghost;
        Boxes boxes;
        sf::Vector2i win_tile;
        Grid backGround;
        Logic logic;
        bool solved = false;
        bool can_swap = true;

        sf::Font font;
        std::string name;
        int text_lines;
        std::vector<std::string> text;

        std::map<char, sf::Texture> bg_tiles;
        std::map<std::string, sf::Texture> flag_icons;

        bool boxPush(Entity* pusher, char direction, int* nb_pushed);//Recursive calls of the pushing function
        void pLikePush(PlayerLike* pushed, char direction);//End call for the pushing function : if a box pushes an enemy
        //(or the player but it's not currently possible)

        void displayBG(sf::RenderWindow * windowP);

    public:
        int nbSteps = 0;
        int perf_steps;
        bool won = false;

        Level();
        Level(std::string file_name, sf::Font font);
        Level(int number, bool solved, sf::Font font);
        std::string get_pLike_state();  //To record the states of the Player and ghost into the list of steps

        bool isWallForMost(sf::Vector2i coords);    //"Most" being the player and boxes
        bool isWallForGhost(sf::Vector2i coords);
        bool push(char direction, std::string* act);  //First call of the pushing recursive function (the player pushes)
        bool swap(std::string* act);
        bool wait();
        void undo(std::list<std::string>* steps);
        void process_logic(bool didSwap);
        void step(bool didSwap);
        void validate_step();

        void resize_bg(sf::RenderWindow* windowP);
        void display(sf::RenderWindow* windowP, bool disp = true, int frame = 4);
        void animate(sf::RenderWindow* windowP);
};

void display_pause(sf::RenderWindow*, sf::Font);    // Function to display the pause menu over the level
int pause(Level*, sf::RenderWindow*, sf::Font);     // Function to handle pause menu interractions
int run(int, bool solved, sf::RenderWindow*, sf::Font, int*);          // Function replacing the original main loop of swap.cpp

#endif //LEVEL_H