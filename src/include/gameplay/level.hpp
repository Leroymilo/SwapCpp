#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <vector>
#include <list>

#include "globals.hpp"
#include "gameplay/grid.hpp"
#include "gameplay/entities.hpp"
#include "gameplay/logic.hpp"
#include "UI/button.hpp"

std::string make_level_name(int nb);

class Level
{
    private:
        // Stuff for background display
        bool solved = false;
        std::map<char, sf::Texture> bg_tiles;
        std::string name;   // hidden for now
        int hint_lines = 0;
        std::vector<std::string> hint;
        int dlg_lines = 0;
        std::vector<std::string> dlg;

        // Level elements
        SwapperEntity player;
        std::vector<BaseEntity> statues;
        Logic logic;
        Grid backGround;
        sf::Vector2i win_tile;

        // Level flags, dictating how the game plays
        std::map<std::string, bool> flags;
        std::map<std::string, sf::Texture> flag_textures;
        std::map<std::string, sf::Sprite> flag_sprites;
        std::list<std::string> flag_order = {"has_ghost", "can_swap"};


        bool get_statue(sf::Vector2i, BaseEntity*&);
        bool get_physf(sf::Vector2i, MovingEntity*&);
        bool get_ghost(sf::Vector2i, MovingEntity*&);
        bool is_wall_for_physf(sf::Vector2i);
        bool is_wall_for_ghost(sf::Vector2i);

        bool base_push(BaseEntity* pusher, Direction);     // A BaseEntity pushes
        bool physf_push(MovingEntity* pusher, Direction);  // A physical MovingEntity pushes
        bool ghost_push(MovingEntity* pusher, Direction);  // A ghostly MovingEntity pushes
        // They all return true if the pusher was able to push, false else

        void displayBG(sf::RenderWindow * windowP);

    public:
        int nbSteps = 0;
        int perf_steps;
        bool won = false;

        Level();
        Level(std::string file_name);
        Level(int number, bool solved);

        bool player_push(Direction);  //First call of the pushing recursive function (the player pushes)
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

void display_pause(sf::RenderWindow*);    // Function to display the pause menu over the level
int pause(Level*, sf::RenderWindow*);     // Function to handle pause menu interractions
int run(int, bool solved, sf::RenderWindow*, int*);          // Function replacing the original main loop of swap.cpp

#endif //LEVEL_H