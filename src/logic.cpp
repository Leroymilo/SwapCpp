#include "gameplay/logic.hpp"

#include <json/value.h>

#include <fstream>
#include <iostream>

std::map<char, int> gate_dirs = {{'U', 0}, {'R', 1}, {'D', 2}, {'L', 3}};
std::map<char, int> door_orients = {{'N', 0}, {'H', 1}, {'V', 2}};

// Methods for Link
Link::Link(){}

Link::Link(Json::Value json_link)
{
    nb_nodes = json_link["nb_nodes"].asInt();
    nodes.resize(nb_nodes);
    offsets.resize(nb_nodes-1);
    for (int i = 0; i < nb_nodes; i++)
    {
        if (i < nb_nodes - 1) {offsets[i] = json_link["offsets"][i].asInt();}
        nodes[i] = sf::Vector2i(json_link["nodes"][i]["X"].asInt(), json_link["nodes"][i]["Y"].asInt());
    }
    
    input_type = json_link["type_start"].asCString();
    output_type = json_link["type_end"].asCString();
}

sf::Vector2i Link::get_input()
{
    return nodes[0];
}

sf::Vector2i Link::get_output()
{
    return nodes[nb_nodes-1];
}

void Link::set_state(bool new_state)
{
    state = new_state;
}

bool Link::get_state()
{
    return state;
}

void Link::step_end_logic()
{
    prev_states.push_back(state);
}

void Link::undo()
{
    prev_states.pop_back();
    state = prev_states.back();
    prev_states.pop_back();
}

int Link::get_offset(int i)
{
    if (i < 0 || i >= nb_nodes - 1) return 0;
    return offsets[i];
}

void Link::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint)
{
    int thick = delta/16;

    sf::Color color;
    if (state)
        color = sf::Color(255, 127, 39);
    else 
        color = sf::Color(61, 176, 254);
    
    for (int i = 0; i < nb_nodes - 1; i++)
    {
        sf::Vector2i n1 = nodes[i], n2 = nodes[i+1];
        float W, H;
        float X = C0.x, Y = C0.y;

        if (n1.x == n2.x)
        {
            X += delta * n1.x + thick * (7 + get_offset(i));
            W = thick;
            if (n1.y < n2.y)
            {
                Y += delta * n1.y + thick * (7 + get_offset(i-1));
                H = delta * (n2.y - n1.y) + thick * (1 - get_offset(i-1) + get_offset(i+1));
            }
            else
            {
                Y += delta * n2.y + thick * (7 + get_offset(i+1));
                H = delta * (n1.y - n2.y) + thick * (1 - get_offset(i+1) + get_offset(i-1));
            }
        }
        else if (n1.y == n2.y)
        {
            Y += delta * n1.y + thick * (7 + get_offset(i));
            H = thick;
            if (n1.x < n2.x)
            {
                X += delta * n1.x + thick * (7 + get_offset(i-1));
                W = delta * (n2.x - n1.x) + thick * (1 - get_offset(i-1) + get_offset(i+1));
            }
            else
            {
                X += delta * n2.x + thick * (7 + get_offset(i+1));
                W = delta * (n1.x - n2.x) + thick * (1 - get_offset(i+1) + get_offset(i-1));
            }
        }
        else continue;  //I'm not drawing a diagonal rectangle, fuck you.

        sf::RectangleShape line(sf::Vector2f(W, H));
        line.setPosition(sf::Vector2f(X, Y));
        line.setFillColor(color);
        windowPoint->draw(line);
    }
}

// Methods for Activator
Activator::Activator(){}

Activator::Activator(char type)
{
    this->type = type;

    std::string name;
    if (type=='I')
        name = "Button";
    else if (type=='T')
        name = "Target";
    
    sprites.resize(5);
    for (int i = 0; i < 5; i++)
    {
        sprites[i].loadFromFile(
            "assets/Logic/" + name + ".png",
            sf::IntRect(i*16, 0, 16, 16)
        );
    }
}

void Activator::add_output(int id_link)
{
    outputs.push_back(id_link);
}

void Activator::get_outputs(std::vector<sf::Vector2i> * to_update, std::map<int, Link> * links)
{
    for (auto &id_link : outputs)
    {
        links[0][id_link].set_state(state);

        sf::Vector2i new_update = links[0][id_link].get_output();
        //Checking if the new element to update is already in the vector :
        if (std::find(to_update->begin(), to_update->end(), new_update) == to_update->end())
            to_update->push_back(new_update);
    }
}

void Activator::set_state(bool new_state)
{
    state = new_state;
}

bool Activator::get_state()
{
    return state;
}

void Activator::step_end_logic()
{
    prev_states.push_back(state);
}

void Activator::undo()
{
    prev_states.pop_back();
    state = prev_states.back();
    prev_states.pop_back();
}

sf::RectangleShape Activator::draw(int delta)
{
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    tile.setTexture(&sprites[state*4]);
    return tile;
}

sf::RectangleShape Activator::anim(int delta, int frame)
{
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    if (state && !(prev_states.back()))
        tile.setTexture(&sprites[frame]);
    else if (!state && (prev_states.back()))
        tile.setTexture(&sprites[4-frame]);
    else
        tile.setTexture(&sprites[state*4]);
    return tile;
}

// Methods for Gate
Gate::Gate(){}

Gate::Gate(Json::Value gate_data)
{
    type = gate_data["type"].asCString();
    int dir = gate_dirs.find(gate_data["dir"].asCString()[0])->second;
    for (int i = 0; i < 5; i++)
    {
        sprites[i].loadFromFile(
            "assets/Logic/" + type + ".png",
            sf::IntRect(i*16, dir*16, 16, 16)
        );
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

    if (type=="AND")
    {
        new_state = true;

        for (auto &id_link : inputs)
        {
            if (!links[0][id_link].get_state())
            {
                new_state = false;
                break;
            }
        }
    }

    else if (type=="OR")
    {
        new_state = false;

        for (auto &id_link : inputs)
        {
            if (links[0][id_link].get_state())
            {
                new_state = true;
                break;
            }
        }
    }

    else if (type=="NO")
        new_state = !links[0][inputs[0]].get_state();

    bool changed = state != new_state;
    state = new_state;

    return changed;
}

void Gate::get_outputs(std::vector<sf::Vector2i> * to_update, std::map<int, Link> * links)
{
    for (auto &id_link : outputs)
    {
        links[0][id_link].set_state(state);

        sf::Vector2i new_update = links[0][id_link].get_output();
        //Checking if the new element to update is already in the vector :
        if (std::find(to_update->begin(), to_update->end(), new_update) == to_update->end())
            to_update->push_back(new_update);
    }
}

void Gate::step_end_logic()
{
    prev_states.push_back(state);
}

void Gate::undo()
{
    prev_states.pop_back();
    state = prev_states.back();
    prev_states.pop_back();
}

sf::RectangleShape Gate::draw(int delta)
{
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    tile.setTexture(&sprites[state*4]);
    return tile;
}

sf::RectangleShape Gate::anim(int delta, int frame)
{
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    if (state && !prev_states.back())
        tile.setTexture(&sprites[frame]);
    else if (!state && prev_states.back())
        tile.setTexture(&sprites[4-frame]);
    else
        tile.setTexture(&sprites[state*4]);
    return tile;
}

// Methods for Door
Door::Door(){}

Door::Door(Json::Value door_data)
{ 
    // Reading tiles :
    nb_tiles = door_data["nb_tiles"].asInt();
    for (int i = 0; i < nb_tiles; i++)
    {
        Json::Value tile_data = door_data["tiles"][i];
        sf::Vector2i C(tile_data["X"].asInt(), tile_data["Y"].asInt());
        tiles[C] = tile_data["orient"].asCString()[0];
    }

    // Loading spritesheet :
    for (auto &elt : door_orients)
    {
        char orient = elt.first;
        int i = elt.second;
        sprites[orient].resize(5);
        for (int j = 0; j < 5; j++)
        {
            sprites[orient][j].loadFromFile(
                "assets/Logic/Door.png",
                sf::IntRect(j*16, i*16, 16, 16)
            );
        }
    }
}

void Door::add_input(int id_link)
{
    input = id_link;
}

std::vector<sf::Vector2i> Door::get_tiles_pos()
{
    std::vector<sf::Vector2i> tiles_pos;
    for (auto &tile_elt : tiles)
        tiles_pos.push_back(tile_elt.first);
    return tiles_pos;
}

void Door::update_state(std::map<int, Link> * links)
{
    state = links[0][input].get_state();
}

bool Door::get_state()
{
    return state;
}

void Door::step_end_logic()
{
    prev_states.push_back(state);
}

void Door::undo()
{
    prev_states.pop_back();
    state = prev_states.back();
    prev_states.pop_back();
}

void Door::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint)
{
    sf::RectangleShape rectangle(sf::Vector2f(delta, delta));
    for (auto &tile_elt : tiles)
    {
        rectangle.setTexture(&sprites[tile_elt.second][4*state]);
        rectangle.setPosition(C0.x + tile_elt.first.x * delta, C0.y + tile_elt.first.y * delta);
        windowPoint->draw(rectangle);
    }
}

void Door::anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame)
{
    int sprite_x;
    if (state && !prev_states.back())
        sprite_x = frame;
    else if (prev_states.back() && !state)
        sprite_x = 4 - frame;
    else
        sprite_x = 4 * state;
    
    sf::RectangleShape rectangle(sf::Vector2f(delta, delta));
    for (auto &tile_elt : tiles)
    {
        rectangle.setTexture(&sprites[tile_elt.second][sprite_x]);
        rectangle.setPosition(C0.x + tile_elt.first.x * delta, C0.y + tile_elt.first.y * delta);
        windowPoint->draw(rectangle);
    }
}

// Methods for Logic
Logic::Logic(){}

Logic::Logic(Json::Value json_logic)
{
    std::vector<sf::Vector2i> to_update;

    int nb_activators = json_logic["nb_activators"].asInt();
    for (int i = 0; i < nb_activators; i++)
    {
        sf::Vector2i C = sf::Vector2i(json_logic["activators"][i]["X"].asInt(), json_logic["activators"][i]["Y"].asInt());
        activators[C] = Activator(*json_logic["activators"][i]["type"].asCString());
    }

    int nb_gates = json_logic["nb_gates"].asInt();
    for (int i = 0; i < nb_gates; i++)
    {
        sf::Vector2i C = sf::Vector2i(json_logic["gates"][i]["X"].asInt(), json_logic["gates"][i]["Y"].asInt());
        gates[C] = Gate(json_logic["gates"][i]);

        if (gates[C].type == "NO")
            to_update.push_back(C);
    }

    int nb_doors = json_logic["nb_doors"].asInt();
    for (int i = 0; i < nb_doors; i++)
    {
        sf::Vector2i C = sf::Vector2i(json_logic["doors"][i]["X"].asInt(), json_logic["doors"][i]["Y"].asInt());
        doors[C] = Door(json_logic["doors"][i]);
        for (sf::Vector2i &tile_pos : doors[C].get_tiles_pos())
        {
            door_tiles[tile_pos] = C;
        }
    }

    int nb_links = json_logic["nb_links"].asInt();
    for (int i = 0; i < nb_links; i++)
    {
        Link link(json_logic["links"][i]);

        std::string type_start = json_logic["links"][i]["type_start"].asCString();
        std::string type_end = json_logic["links"][i]["type_end"].asCString();
        sf::Vector2i C_start = link.get_input(), C_end = link.get_output();

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
    step_end_logic();
}

bool Logic::isClosedDoor(sf::Vector2i coords)
{
    auto elt_door_tile = door_tiles.find(coords);
    if (elt_door_tile != door_tiles.end())
        return !doors[elt_door_tile->second].get_state();
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

std::vector<sf::Vector2i> Logic::update_activators(
    std::vector<sf::Vector2i> heavy_coords,
    std::vector<sf::Vector2i> arrow_coords,
    bool didSwap, bool * balive)
{
    //heavy_coords has the coordinates of "heavy objects" (alive boxes and the player if it's alive)
    //arrow_coords has the coordinates of "arrow objects" (just the bullet if it's alive)

    std::vector<sf::Vector2i> changed;

    for (auto &act_elt_p : activators)
    {
        if (act_elt_p.second.type == 'I')
        {
            if (std::find(heavy_coords.begin(), heavy_coords.end(), act_elt_p.first) != heavy_coords.end())
            {
                if (!act_elt_p.second.get_state())
                {
                    activators[act_elt_p.first].set_state(true);
                    activators[act_elt_p.first].get_outputs(&changed, &links);
                }
            }

            else if (act_elt_p.second.get_state())
            {
                activators[act_elt_p.first].set_state(false);
                activators[act_elt_p.first].get_outputs(&changed, &links);
            }
        }

        else if (act_elt_p.second.type == 'T')
        {
            auto elt_act = std::find(arrow_coords.begin(), arrow_coords.end(), act_elt_p.first);

            if (elt_act != arrow_coords.end())
            {
                if (!act_elt_p.second.get_state())
                {
                    activators[act_elt_p.first].set_state(true);
                    activators[act_elt_p.first].get_outputs(&changed, &links);
                }
                *balive = false;
            }

            else if (act_elt_p.second.get_state() && didSwap)
            {
                activators[act_elt_p.first].set_state(false);
                activators[act_elt_p.first].get_outputs(&changed, &links);
            }
        }
    }

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
        auto gate_elt = gates.find(coords);
        if (gate_elt != gates.end())
        {
            bool has_changed = gates[gate_elt->first].update_state(&links);
            if (has_changed)
                gates[gate_elt->first].get_outputs(&new_elts, &links);
        }

        auto door_elt = doors.find(coords);
        if (door_elt != doors.end())
        {
            doors[door_elt->first].update_state(&links);
        }

        //Having 2 logic elements on the same square is not allowed
    }

    update(new_elts);
}

void Logic::step_end_logic()
{
    for (auto &link_elt : links)
        link_elt.second.step_end_logic();
    for (auto &act_elt : activators)
        act_elt.second.step_end_logic();
    for (auto &gate_elt : gates)
        gate_elt.second.step_end_logic();
    for (auto &door_elt : doors)
        door_elt.second.step_end_logic();
}

void Logic::undo()
{
    for (auto &link_elt : links)
        link_elt.second.undo();
    for (auto &act_elt : activators)
        act_elt.second.undo();
    for (auto &gate_elt : gates)
        gate_elt.second.undo();
    for (auto &door_elt : doors)
        door_elt.second.undo();
}

void Logic::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint)
{
    for (auto &link_elt : links)
        link_elt.second.draw(C0, delta, windowPoint);

    for (auto &elt : activators)
    {
        sf::Vector2i C = elt.first;
        sf::RectangleShape tile = elt.second.draw(delta);
        tile.setPosition(C.x*delta+C0.x, C.y*delta+C0.y);
        windowPoint->draw(tile);
    }

    for (auto &elt : gates)
    {
        sf::Vector2i C = elt.first;
        sf::RectangleShape tile = elt.second.draw(delta);
        tile.setPosition(C.x*delta+C0.x, C.y*delta+C0.y);
        windowPoint->draw(tile);
    }

    for (auto &elt : doors)
        elt.second.draw(C0, delta, windowPoint);
}

void Logic::anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame)
{
    
    for (auto &link_elt : links)
        link_elt.second.draw(C0, delta, windowPoint);

    for (auto &elt : activators)
    {
        sf::Vector2i C = elt.first;
        sf::RectangleShape tile = elt.second.anim(delta, frame);
        tile.setPosition(C.x*delta+C0.x, C.y*delta+C0.y);
        windowPoint->draw(tile);
    }

    for (auto &elt : gates)
    {
        sf::Vector2i C = elt.first;
        sf::RectangleShape tile = elt.second.anim(delta, frame);
        tile.setPosition(C.x*delta+C0.x, C.y*delta+C0.y);
        windowPoint->draw(tile);
    }

    for (auto &elt : doors)
    {
        elt.second.anim(C0, delta, windowPoint, frame);
    }
}