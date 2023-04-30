#ifndef LOGIC_H
#define LOGIC_H

#include <list>
#include <vector>
#include <unordered_set>

#include <json/value.h>

#include "globals.hpp"

class Link
{
    private :
        int nb_nodes;
        std::vector<sf::Vector2i> nodes;
        std::vector<int> offsets;
        sf::Vector2i input;
        sf::Vector2i output;
        std::string input_type;
        std::string output_type;
        // Types are "Activator", "Gate" or "Door". "Activator" cannot be output and "Door" cannot be input.
        bool state = false;
        std::list<bool> hist_states = {false};

        int get_offset(int i);
    
    public :
        Link();
        Link(Json::Value json_link);
        sf::Vector2i get_input();
        sf::Vector2i get_output();
        void set_state(bool new_state);
        bool get_state();
        void validate_step();
        void undo();

        void draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, sf::Texture *texture);
};

class Activator
{
    private :
        std::vector<sf::Texture> sprites;
        std::list<int> outputs;
        bool state = false;
        std::list<bool> hist_states = {false};
    
    public :
        char type;

        Activator();
        Activator(char type);

        void add_output(int id_link);
        void get_outputs(std::list<sf::Vector2i> * to_update, std::map<int, Link> * links);
        void set_state(bool new_state);
        bool get_state();
        void validate_step();
        void undo();

        sf::RectangleShape draw(int delta);
        sf::RectangleShape anim(int delta, int frame);
};

class Gate
{
    private :
        sf::Texture sprites[5];
        std::list<int> inputs;
        std::list<int> outputs;
        bool state = false;
        std::list<bool> hist_states = {false};
    
    public :
        std::string type;

        Gate();
        Gate(Json::Value gate_data);

        void add_output(int id_link);
        void add_input(int id_link);
        bool update_state(std::map<int, Link> * links);
        //Returns true if output changed, does not update states of the output links.
        void get_outputs(std::list<sf::Vector2i> * to_update, std::map<int, Link> * links);
        //Updates states of the output links and add positions of outputs's end points to to_update.
        void validate_step();
        void undo();

        sf::RectangleShape draw(int delta);
        sf::RectangleShape anim(int delta, int frame);
};

class Door
{
    private :
        int input;
        int nb_tiles;
        std::map<sf::Vector2i, int, VectorComparator> tiles;    // map of {tile coords, tile orientation}
        bool state = false;                                     //false is closed, true is open
        std::list<bool> hist_states = {false};
        std::map<char, std::vector<sf::Texture>> sprites;
    
    public :
        char type = 'D';

        Door();
        Door(Json::Value door_data);
        void add_input(int id_link);
        std::list<sf::Vector2i> get_tiles_pos();

        void update_state(std::map<int, Link> * links);
        bool get_state();
        void validate_step();
        void undo();

        void draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint);
        void anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame);
};

class Logic
{
    private :
        std::map<int, Link> links;
        std::map<sf::Vector2i, Activator, VectorComparator> activators;
        std::map<sf::Vector2i, Gate, VectorComparator> gates;
        std::map<sf::Vector2i, Door, VectorComparator> doors;
        std::map<sf::Vector2i, sf::Vector2i, VectorComparator> door_tiles; //Used to search for the state of a doortile faster for collisions

        std::vector<sf::Texture> link_textures;
    
    public :
        Logic();
        Logic(Json::Value json_logic);
        bool is_closed_door(sf::Vector2i coords);
        bool is_wall_for_physf(sf::Vector2i coords);    //"Most" being the player and boxes
        bool is_wall_for_ghost(sf::Vector2i coords);

        std::list<sf::Vector2i> update_activators(
            std::unordered_set<sf::Vector2i, VectorHasher> heavy_coords,
            std::unordered_set<sf::Vector2i, VectorHasher> ghost_coords,
            bool did_swap, bool * ghost_alive
        );
        //Returns the position of the outputs of every activator that chenged state
        void update(std::list<sf::Vector2i> changed_elts);
        void validate_step();
        void undo();

        void draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint);
        void anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame);
};

#endif //LOGIC_H