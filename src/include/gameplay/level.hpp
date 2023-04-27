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

        std::map<std::string, bool> flags;
        std::map<std::string, sf::Texture> flag_icons;
        std::list<std::string> ordered_flags;

        sf::Font font;
        std::string name;
        int hint_lines = 0;
        std::vector<std::string> hint = {};
        int dlg_lines = 0;
        std::vector<std::string> dlg = {};

        std::map<char, sf::Texture> bg_tiles;

        bool boxPush(Entity* pusher, char direction);//Recursive calls of the pushing function
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
        bool push(char direction);  //First call of the pushing recursive function (the player pushes)
        bool swap();
        bool wait();
        void undo(std::list<char>* steps);
        void process_logic(bool didSwap);
        void step(bool didSwap);
        void validate_step();

        void resize_bg(sf::RenderWindow* windowP);
        void display(sf::RenderWindow* windowP, bool disp = true);
        void animate(sf::RenderWindow* windowP, int frame, bool disp = true);
};

void display_pause(sf::RenderWindow*, sf::Font);    // Function to display the pause menu over the level
int pause(Level*, sf::RenderWindow*, sf::Font);     // Function to handle pause menu interractions
int run(int, bool solved, sf::RenderWindow*, sf::Font, int*);          // Function replacing the original main loop of swap.cpp

#endif //LEVEL_H