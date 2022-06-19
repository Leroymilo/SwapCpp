#include "logic.hpp"

#include <json/value.h>
#include <json/json.h>

#include <fstream>
#include <iostream>

// Methods for Link
Link::Link(){}

Link::Link(sf::Vector2i input, sf::Vector2i output, std::string input_type, std::string output_type)
{
    this->input = input;
    this->output = output;
    this->input_type = input_type;
    this->output_type = output_type;
}

sf::Vector2i Link::get_output()
{
    return output;
}

void Link::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint)
{
    sf::Color color;
    if (state)
        color = sf::Color(255, 127, 39);
    else 
        color = sf::Color(61, 176, 254);
    
    sf::VertexArray lines(sf::LinesStrip, 2);
    lines[0].position = sf::Vector2f(C0.x+(input.x+0.5)*delta, C0.y+(input.y+0.5)*delta);
    lines[1].position = sf::Vector2f(C0.x+(output.x+0.5)*delta, C0.y+(output.y+0.5)*delta);
    lines[0].color = color;
    lines[1].color = color;

    windowPoint->draw(lines);
}

// Methods for Activator
Activator::Activator(){}

Activator::Activator(char type)
{
    this->type = type;

    std::string name;
    if (type=='I')
        name = "Interruptor";
    else if (type=='T')
        name = "Target";
    
    for (int i = 0; i < 5; i++)
    {
        sf::Texture sprite;
        std::ostringstream oss;
        oss << "assets/Logic/" << name << i << ".png";
        sprite.loadFromFile(oss.str());
        sprites[i] = sprite;
    }
}

void Activator::add_output(int id_link)
{
    outputs.push_back(id_link);
}

void Activator::get_outputs(std::vector<sf::Vector2i> * to_update, std::map<int, Link> * links)
{
    for (auto id_link : outputs)
    {
        links[0][id_link].state = state;

        sf::Vector2i new_update = links[0][id_link].get_output();
        //Checking if the new element to update is already in the vector :
        if (std::find(to_update->begin(), to_update->end(), new_update) == to_update->end())
            to_update->push_back(new_update);
    }
}

sf::RectangleShape Activator::draw(int delta)
{
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    tile.setTexture(&sprites[state*4]);
    return tile;
}

sf::RectangleShape Activator::anim(bool prevState, int delta, int frame)
{
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    if (state && !prevState)
        tile.setTexture(&sprites[frame]);
    else if (!state && prevState)
        tile.setTexture(&sprites[4-frame]);
    else
        tile.setTexture(&sprites[state*4]);
    return tile;
}

// Methods for Gate
Gate::Gate(){}

Gate::Gate(char type)
{
    this->type = type;

    std::string name;
    if (type=='!')
        name = "NO";
    else if (type=='|')
        name = "OR";
    else if (type=='&')
        name = "AND";
    
    for (int i = 0; i < 5; i++)
    {
        sf::Texture sprite;
        std::ostringstream oss;
        oss << "assets/Logic/" << name << i << ".png";
        sprite.loadFromFile(oss.str());
        sprites[i] = sprite;
    }
}

void Gate::add_output(int id_link)
{
    outputs.push_back(id_link);
}

void Gate::add_input(int id_link)
{
    inputs.push_back(id_link);
}

bool Gate::update_state(std::map<int, Link> * links)
{
    bool new_state;

    if (type=='&')  //AND gate
    {
        new_state = true;

        for (auto id_link : inputs)
        {
            if (!links[0][id_link].state)
            {
                new_state = false;
                break;
            }
        }
    }

    else if (type=='|') //OR gate
    {
        new_state = false;

        for (auto id_link : inputs)
        {
            if (links[0][id_link].state)
            {
                new_state = true;
                break;
            }
        }
    }

    else if (type=='!') //NOT gate
        new_state = !links[0][inputs[0]].state;

    bool changed = state != new_state;
    state = new_state;
    return changed;
}

void Gate::get_outputs(std::vector<sf::Vector2i> * to_update, std::map<int, Link> * links)
{
    for (auto id_link : outputs)
    {
        links[0][id_link].state = state;

        sf::Vector2i new_update = links[0][id_link].get_output();
        //Checking if the new element to update is already in the vector :
        if (std::find(to_update->begin(), to_update->end(), new_update) == to_update->end())
            to_update->push_back(new_update);
    }
}

sf::RectangleShape Gate::draw(int delta)
{
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    tile.setTexture(&sprites[state*4]);
    return tile;
}

sf::RectangleShape Gate::anim(bool prevState, int delta, int frame)
{
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    if (state && !prevState)
        tile.setTexture(&sprites[frame]);
    else if (!state && prevState)
        tile.setTexture(&sprites[4-frame]);
    else
        tile.setTexture(&sprites[state*4]);
    return tile;
}

// Methods for DoorTile
DoorTile::DoorTile(){}

DoorTile::DoorTile(sf::Vector2i pos)
{
    this->pos = pos;
    //TODO : add hinges
}

sf::Vector2i DoorTile::getPos()
{
    return pos;
}

// Methods for Door
Door::Door(){}

Door::Door(bool arg)
{ 
    for (int i = 0; i < 5; i++)
    {
        sf::Texture sprite;
        std::ostringstream oss;
        oss << "assets/Logic/Door" << i << ".png";
        sprite.loadFromFile(oss.str());
        sprites[i] = sprite;
    }
}

void Door::add_tile(DoorTile tile)
{
    tiles.push_back(tile);
}

void Door::add_input(int id_link)
{
    input = id_link;
}

std::vector<sf::Vector2i> Door::get_tiles_pos()
{
    std::vector<sf::Vector2i> tiles_pos;
    for (auto tile : tiles)
        tiles_pos.push_back(tile.getPos());
    return tiles_pos;
}

void Door::update_state(std::map<int, Link> * links)
{
    state = links[0][input].state;
}

void Door::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint)
{
    for (auto tile : tiles)
    {
        sf::RectangleShape rectangle(sf::Vector2f(delta, delta));
        rectangle.setTexture(&sprites[state*4]);
        rectangle.setPosition(tile.getPos().x*delta+C0.x, tile.getPos().y*delta+C0.y);
        windowPoint->draw(rectangle);
    }
}

void Door::anim(bool prevState, sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame)
{
    for (auto tile : tiles)
    {
        sf::RectangleShape rectangle(sf::Vector2f(delta, delta));
        if (state && !prevState)
            rectangle.setTexture(&sprites[frame]);
        else if (prevState && !state)
            rectangle.setTexture(&sprites[4-frame]);
        else 
            rectangle.setTexture(&sprites[state*4]);
        rectangle.setPosition(tile.getPos().x*delta+C0.x, tile.getPos().y*delta+C0.y);
        windowPoint->draw(rectangle);
    }
}

// Methods for Logic
Logic::Logic(){}

Logic::Logic(std::string directory)
{
    std::ifstream file(directory);
    Json::Value actualJson;
    Json::Reader reader;

    reader.parse(file, actualJson);

    std::vector<sf::Vector2i> to_update;

    int nb_activators = actualJson["nb_activators"].asInt();
    for (int i = 0; i < nb_activators; i++)
    {
        sf::Vector2i C = sf::Vector2i(actualJson["activators"][i]["X"].asInt(), actualJson["activators"][i]["Y"].asInt());
        activators[C] = Activator(*actualJson["activators"][i]["type"].asCString());
    }

    int nb_gates = actualJson["nb_gates"].asInt();
    for (int i = 0; i < nb_gates; i++)
    {
        sf::Vector2i C = sf::Vector2i(actualJson["gates"][i]["X"].asInt(), actualJson["gates"][i]["Y"].asInt());
        char type = actualJson["gates"][i]["type"].asCString()[0];
        gates[C] = Gate(type);
        if (type == '!')
            to_update.push_back(C);
    }

    int nb_doors = actualJson["nb_doors"].asInt();
    for (int i = 0; i < nb_doors; i++)
    {
        sf::Vector2i C = sf::Vector2i(actualJson["doors"][i]["X"].asInt(), actualJson["doors"][i]["Y"].asInt());
        doors[C] = Door(true);
        int nb_tiles = actualJson["doors"][i]["nb_tiles"].asInt();
        for (int j = 0; j < nb_tiles; j++)
        {
            int X = actualJson["doors"][i]["tiles"][j]["X"].asInt();
            int Y = actualJson["doors"][i]["tiles"][j]["Y"].asInt();
            doors[C].add_tile(sf::Vector2i(X, Y));
            door_tiles[sf::Vector2i(X, Y)] = C;
        }
    }

    int nb_links = actualJson["nb_links"].asInt();
    for (int i = 0; i < nb_links; i++)
    {
        std::string type_start = actualJson["links"][i]["type_start"].asCString();
        sf::Vector2i C_start = sf::Vector2i(actualJson["links"][i]["X_start"].asInt(), actualJson["links"][i]["Y_start"].asInt());
        std::string type_end = actualJson["links"][i]["type_end"].asCString();
        sf::Vector2i C_end = sf::Vector2i(actualJson["links"][i]["X_end"].asInt(), actualJson["links"][i]["Y_end"].asInt());

        Link link(C_start, C_end, type_start, type_end);

        links[i] = link;

        if (type_start == "Activator")
            activators[C_start].add_output(i);
        else
            gates[C_start].add_output(i);

        if (type_end == "Gate")
            gates[C_end].add_input(i);
        else
            doors[C_end].add_input(i);
    }

    //Updating logic because of no gates :

    update(to_update);

    //Debug :

    // std::cout << "activators :" << std::endl;
    // for (auto act_elt : activators)
    //     std::cout << act_elt.first.x << " " << act_elt.first.y << std::endl;

    // std::cout << std::endl << "gates :" << std::endl;
    // for (auto gate_elt : gates)
    //     std::cout << gate_elt.first.x << " " << gate_elt.first.y << std::endl;

    // std::cout << std::endl << "doors :" << std::endl;
    // for (auto door_elt : doors)
    //     std::cout << door_elt.first.x << " " << door_elt.first.y << std::endl;

}

bool Logic::isClosedDoor(sf::Vector2i coords)
{
    auto elt_door_tile = door_tiles.find(coords);
    if (elt_door_tile != door_tiles.end())
        return !doors[elt_door_tile->second].state;
    return false;
}

bool Logic::isWallForMost(sf::Vector2i coords)
{
    auto elt_act = activators.find(coords);
    if (elt_act != activators.end() && elt_act->second.type == 'T')
        return true;

    return isClosedDoor(coords);
}

bool Logic::isWallForBullet(sf::Vector2i coords)
{
    return isClosedDoor(coords);
}

std::vector<sf::Vector2i> Logic::update_activators(std::vector<sf::Vector2i> heavy_coords, std::vector<sf::Vector2i> arrow_coords, bool didSwap, bool * balive)
{
    //heavy_coords has the coordinates of "heavy objects" (alive boxes and the player if it's alive)
    //arrow_coords has the coordinates of "arrow objects" (just the bullet if it's alive)

    std::vector<sf::Vector2i> changed;

    for (auto act_elt_p : activators)
    {
        if (act_elt_p.second.type == 'I')
        {
            if (std::find(heavy_coords.begin(), heavy_coords.end(), act_elt_p.first) != heavy_coords.end())
            {
                if (!act_elt_p.second.state)
                {
                    activators[act_elt_p.first].state = true;
                    activators[act_elt_p.first].get_outputs(&changed, &links);
                }
            }

            else if (act_elt_p.second.state)
            {
                activators[act_elt_p.first].state = false;
                activators[act_elt_p.first].get_outputs(&changed, &links);
            }
        }

        else if (act_elt_p.second.type == 'T')
        {
            if (act_elt_p.second.state && didSwap)
            {
                activators[act_elt_p.first].state = false;
                activators[act_elt_p.first].get_outputs(&changed, &links);
            }
            
            else if (!act_elt_p.second.state && std::find(arrow_coords.begin(), arrow_coords.end(), act_elt_p.first) != arrow_coords.end())
            {
                activators[act_elt_p.first].state = true;
                activators[act_elt_p.first].get_outputs(&changed, &links);
                *balive = false;
            }
        }
    }

    // std::cout << "changed positions :" << std::endl;
    // for (auto pos : changed)
    // {
    //     std::cout << pos.x << " " << pos.y << std::endl;
    // }

    return changed;
}

void Logic::update(std::vector<sf::Vector2i> changed_elts)
{
    //This method will call itself until stability is reached.
    //It is therefore required to be careful with gates.
    if (changed_elts.size() == 0)
        return;

    std::vector<sf::Vector2i> new_elts;

    for (auto &coords : changed_elts)
    {
        // std::cout << "updating at " << coords.x << " " << coords.y << std::endl;

        auto gate_elt = gates.find(coords);
        if (gate_elt != gates.end())
        {
            // std::cout << "gate at " << gate_elt->first.x << " " << gate_elt->first.y << std::endl;
            bool has_changed = gates[gate_elt->first].update_state(&links);
            if (has_changed)
                gates[gate_elt->first].get_outputs(&new_elts, &links);
        }

        auto door_elt = doors.find(coords);
        if (door_elt != doors.end())
        {
            // std::cout << "door at " << door_elt->first.x << " " << door_elt->first.y << std::endl;
            doors[door_elt->first].update_state(&links);
        }

        //Having 2 logic elements on the same square is not allowed
    }

    // std::cout << "changed positions :" << std::endl;
    // for (auto pos : new_elts)
    // {
    //     std::cout << pos.x << " " << pos.y << std::endl;
    // }
    // std::cout << std::endl;

    update(new_elts);
}

void Logic::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint)
{

    for (auto link_elt : links)
        link_elt.second.draw(C0, delta, windowPoint);

    for (auto elt : activators)
    {
        sf::Vector2i C = elt.first;
        sf::RectangleShape tile = elt.second.draw(delta);
        tile.setPosition(C.x*delta+C0.x, C.y*delta+C0.y);
        windowPoint->draw(tile);
    }

    for (auto elt : gates)
    {
        sf::Vector2i C = elt.first;
        sf::RectangleShape tile = elt.second.draw(delta);
        tile.setPosition(C.x*delta+C0.x, C.y*delta+C0.y);
        windowPoint->draw(tile);
    }

    for (auto elt : doors)
        elt.second.draw(C0, delta, windowPoint);
}

void Logic::anim(Logic prevStep, sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame)
{
    
    for (auto link_elt : links)
        link_elt.second.draw(C0, delta, windowPoint);

    for (auto &elt : activators)
    {
        sf::Vector2i C = elt.first;
        bool prevState = prevStep.activators[C].state;
        sf::RectangleShape tile = elt.second.anim(prevState, delta, frame);
        tile.setPosition(C.x*delta+C0.x, C.y*delta+C0.y);
        windowPoint->draw(tile);
    }

    for (auto &elt : gates)
    {
        sf::Vector2i C = elt.first;
        bool prevState = prevStep.gates[C].state;
        sf::RectangleShape tile = elt.second.anim(prevState, delta, frame);
        tile.setPosition(C.x*delta+C0.x, C.y*delta+C0.y);
        windowPoint->draw(tile);
    }

    for (auto &elt : doors)
    {
        sf::Vector2i C = elt.first;
        bool prevState = prevStep.doors[C].state;
        elt.second.anim(prevState, C0, delta, windowPoint, frame);
    }
}