#ifndef LOGIC_H
#define LOGIC_H

#include <SFML/Graphics.hpp>
#include <list>

//Comparator for a map using sf::Vector2i as keys
//Found here : https://stackoverflow.com/questions/56506121/sfvector2i-as-key-in-stdmap-shows-as-1-1-if-the-y-value-is-1-even-if-the
struct VectorComparator
{
    bool operator() (sf::Vector2i lhs, sf::Vector2i rhs) const
    {
        return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
    }
};

class Link
{
    private :
        sf::Vector2i input;
        std::string input_type;
        sf::Vector2i output;
        std::string output_type;
        // Types are "Activator", "Gate" or "Door". "Activator" cannot be output and "Door" cannot be input.
        bool state = false;
        std::list<bool> prev_states = {false};
    
    public :
        Link();
        Link(sf::Vector2i input, sf::Vector2i output, std::string input_type, std::string output_type);

        sf::Vector2i get_output();
        void set_state(bool new_state);
        bool get_state();
        void step_end_logic();
        void undo();

        void draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint);
};

class Activator
{
    private :
        sf::Texture sprites[5];
        std::vector<int> outputs;
        bool state = false;
        std::list<bool> prev_states;
    
    public :
        char type;

        Activator();
        Activator(char type);

        void add_output(int id_link);
        void get_outputs(std::vector<sf::Vector2i> * to_update, std::map<int, Link> * links);
        void set_state(bool new_state);
        bool get_state();
        void step_end_logic();
        void undo();

        sf::RectangleShape draw(int delta);
        sf::RectangleShape anim(int delta, int frame);
};

class Gate
{
    private :
        sf::Texture sprites[5];
        std::vector<int> inputs;
        std::vector<int> outputs;
        bool state=false;
        std::list<bool> prev_states = {false};
    
    public :
        char type;

        Gate();
        Gate(char type);

        void add_output(int id_link);
        void add_input(int id_link);
        bool update_state(std::map<int, Link> * links);
        //Returns true if output changed, does not update states of the output links.
        void get_outputs(std::vector<sf::Vector2i> * to_update, std::map<int, Link> * links);
        //Updates states of the output links and add positions of outputs's end points to to_update.
        void step_end_logic();
        void undo();

        sf::RectangleShape draw(int delta);
        sf::RectangleShape anim(int delta, int frame);
};

class DoorTile
{
    private :
        sf::Vector2i pos;
        bool hinges[4] = {false, false, false, false};
        //Keeps track of which sides of the door tile are against a wall (Up, Right, Down, Left)
    
    public :
        DoorTile();
        DoorTile(sf::Vector2i pos);
        sf::Vector2i getPos();
};

class Door
{
    private :
        sf::Texture sprites[5];
        int input;
        std::vector<DoorTile> tiles;
        bool state=false;   //false is closed, true is open
        std::list<bool> prev_states = {false};
    
    public :
        char type='D';

        Door();
        Door(bool);
        void add_tile(DoorTile tile);
        void add_input(int id_link);
        std::vector<sf::Vector2i> get_tiles_pos();

        void update_state(std::map<int, Link> * links);
        bool get_state();
        void step_end_logic();
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
    
    public :
        Logic();
        Logic(std::string directory);
        bool isClosedDoor(sf::Vector2i coords);
        bool isWallForMost(sf::Vector2i coords);    //"Most" being the player and boxes
        bool isWallForBullet(sf::Vector2i coords);
        char get_type(sf::Vector2i coords);

        std::vector<sf::Vector2i> update_activators(std::vector<sf::Vector2i> heavy_coords, std::vector<sf::Vector2i> arrow_coords, bool didSwap, bool * balive);
        //Returns the position of the outputs of every activator that chenged state
        void update(std::vector<sf::Vector2i> changed_elts);
        void step_end_logic();
        void undo();

        void draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint);
        void anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame);
};

#endif //LOGIC_H