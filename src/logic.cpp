#include <fstream>
#include <iostream>

#include <json/value.h>

#include "gameplay/logic.hpp"
#include "globals.hpp"

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

void Link::validate_step()
{
    hist_states.push_back(state);
}

void Link::undo()
{
    hist_states.pop_back();
    state = hist_states.back();
}

int Link::get_offset(int i)
{
    if (i < 0 || i >= nb_nodes - 1) return 0;
    return offsets[i];
}

void Link::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, sf::Texture *texture_p)
{
    int thick = delta/8;
    
    for (int i = 0; i < nb_nodes - 1; i++)
    {
        sf::Vector2i n1 = nodes[i], n2 = nodes[i+1];
        float W, H;
        float X = C0.x, Y = C0.y;

        int prev_i, next_i;
        if (n1.x == n2.x)
        {
            X += delta * n1.x + thick * (3.5 + get_offset(i));
            W = thick;

            if (n1.y < n2.y) {prev_i = i-1; next_i = i+1;}
            else {prev_i = i+1; next_i = i-1;}

            Y += delta * __min(n1.y, n2.y) + thick * (3.5 + get_offset(prev_i));
            H = delta * abs(n2.y - n1.y) + thick * (1 - get_offset(prev_i) + get_offset(next_i));
        }
        else if (n1.y == n2.y)
        {
            Y += delta * n1.y + thick * (3.5 + get_offset(i));
            H = thick;
            
            if (n1.x < n2.x) {prev_i = i-1; next_i = i+1;}
            else {prev_i = i+1; next_i = i-1;}

            X += delta * __min(n1.x, n2.x) + thick * (3.5 + get_offset(prev_i));
            W = delta * abs(n2.x - n1.x) + thick * (1 - get_offset(prev_i) + get_offset(next_i));
        }
        else continue;  //I'm not drawing a diagonal rectangle, fuck you.

        // Enlarging the link tile for each pixel to be delta x delta pixels
        sf::RectangleShape link_tile(sf::Vector2f(thick, thick));
        link_tile.setTexture(texture_p);

        // Drawing the link tile into a repeatable RenderTexture
        sf::RenderTexture r_texture;
        r_texture.create(thick, thick);
        r_texture.clear(sf::Color::Transparent);
        r_texture.draw(link_tile);
        r_texture.setRepeated(true);

        // Drawing a sprite with the repeated RenderTexture
        sf::Sprite line(r_texture.getTexture());
        line.setTextureRect(sf::IntRect(0, 0, W, H));
        line.setPosition(X, Y);
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
            sf::IntRect(i*DELTA, 0, DELTA, DELTA)
        );
    }
}

void Activator::add_output(int id_link)
{
    outputs.push_back(id_link);
}

void Activator::get_outputs(std::list<sf::Vector2i> * to_update, std::map<int, Link> * links)
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

void Activator::validate_step()
{
    hist_states.push_back(state);
}

void Activator::undo()
{
    hist_states.pop_back();
    state = hist_states.back();
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
    bool prev_state = *(std::next(hist_states.rbegin()));
    if (state && !prev_state)
        tile.setTexture(&sprites[frame]);
    else if (!state && prev_state)
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
            sf::IntRect(i*DELTA, dir*DELTA, DELTA, DELTA)
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
        new_state = !links[0][inputs.front()].get_state();

    bool changed = state != new_state;
    state = new_state;

    return changed;
}

void Gate::get_outputs(std::list<sf::Vector2i> * to_update, std::map<int, Link> * links)
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

void Gate::validate_step()
{
    hist_states.push_back(state);
}

void Gate::undo()
{
    hist_states.pop_back();
    state = hist_states.back();
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
    bool prev_state = *(std::next(hist_states.rbegin()));
    if (state && !prev_state)
        tile.setTexture(&sprites[frame]);
    else if (!state && prev_state)
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
                sf::IntRect(j*DELTA, i*DELTA, DELTA, DELTA)
            );
        }
    }
}

void Door::add_input(int id_link)
{
    input = id_link;
}

std::list<sf::Vector2i> Door::get_tiles_pos()
{
    std::list<sf::Vector2i> tiles_pos;
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

void Door::validate_step()
{
    hist_states.push_back(state);
}

void Door::undo()
{
    hist_states.pop_back();
    state = hist_states.back();
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
    bool prev_state = *(std::next(hist_states.rbegin()));
    if (state && !prev_state)
        sprite_x = frame;
    else if (!state && prev_state)
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
    std::list<sf::Vector2i> to_update;

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

    //Fetching link textures :
    link_textures.resize(2);
    for (int i=0; i < 2; i++)   // Only 2 states, might add animations
    {
        link_textures[i].loadFromFile(
            "assets/Logic/connector.png",
            sf::IntRect(2 * i, 0, 2, 2)
        );
        // link_textures[i].setRepeated(true);
    }

    //Updating logic because of no gates :

    update(to_update);
    validate_step();
}

bool Logic::is_closed_door(sf::Vector2i coords)
{
    auto elt_door_tile = door_tiles.find(coords);
    if (elt_door_tile != door_tiles.end())
        return !doors[elt_door_tile->second].get_state();
    return false;
}

bool Logic::is_wall_for_physf(sf::Vector2i coords)
{
    auto elt_act = activators.find(coords);
    if (elt_act != activators.end() && elt_act->second.type == 'T')
        return true;

    return is_closed_door(coords);
}

bool Logic::is_wall_for_ghost(sf::Vector2i coords)
{
    return is_closed_door(coords);
}

std::list<sf::Vector2i> Logic::update_activators(
    std::unordered_set<sf::Vector2i, VectorHasher> physf_coords,
    std::unordered_set<sf::Vector2i, VectorHasher> ghost_coords,
    bool did_swap, bool * ghost_alive)
{
    //physf_coords has the coordinates of physical objects (alive boxes and the player if it's alive)
    //ghost_coords has the coordinates of ghost objects (just the ghost if it's alive)

    std::list<sf::Vector2i> changed;

    for (auto &act_elt_p : activators)
    {
        if (act_elt_p.second.type == 'I')
        {
            if (physf_coords.find(act_elt_p.first) != physf_coords.end())
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
            if (ghost_coords.find(act_elt_p.first) != ghost_coords.end())
            {
                if (!act_elt_p.second.get_state())
                {
                    activators[act_elt_p.first].set_state(true);
                    activators[act_elt_p.first].get_outputs(&changed, &links);
                }
                *ghost_alive = false;
            }

            else if (act_elt_p.second.get_state() && did_swap)
            {
                activators[act_elt_p.first].set_state(false);
                activators[act_elt_p.first].get_outputs(&changed, &links);
            }
        }
    }

    return changed;
}

void Logic::update(std::list<sf::Vector2i> changed_elts)
{
    //This method will call itself until stability is reached.
    //It is therefore required to be careful with gates.
    if (changed_elts.size() == 0)
        return;

    std::list<sf::Vector2i> new_elts;

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

void Logic::validate_step()
{
    for (auto &link_elt : links)
        link_elt.second.validate_step();
    for (auto &act_elt : activators)
        act_elt.second.validate_step();
    for (auto &gate_elt : gates)
        gate_elt.second.validate_step();
    for (auto &door_elt : doors)
        door_elt.second.validate_step();
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
    {
        sf::Texture *texture_p = &(link_textures[link_elt.second.get_state()]);
        link_elt.second.draw(C0, delta, windowPoint, texture_p);
    }

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
    {
        sf::Texture *texture_p = &(link_textures[link_elt.second.get_state()]);
        link_elt.second.draw(C0, delta, windowPoint, texture_p);
    }

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