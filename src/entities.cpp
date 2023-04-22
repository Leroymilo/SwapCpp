#include <fstream>
#include <iostream>

#include <json/value.h>
#include <json/json.h>

#include "gameplay/entities.hpp"


//Methods for Entity
Entity::Entity(){}


Entity::Entity(sf::Vector2i pos, sf::Texture sprite) : alive(true), pos(pos), sprite(sprite)
{
    hist_pos.push_back(pos);
    hist_alive.push_back(true);
}

sf::Vector2i Entity::get_next_pos(char direction)
{
    if (direction == 'U')
        return sf::Vector2i(pos.x, pos.y-1);
    else if (direction == 'R')
        return sf::Vector2i(pos.x+1, pos.y);
    else if (direction == 'D')
        return sf::Vector2i(pos.x, pos.y+1);
    else if (direction == 'L')
        return sf::Vector2i(pos.x-1, pos.y);
    return pos;
}

void Entity::validate_step()
{
    hist_pos.push_back(pos);
    hist_alive.push_back(alive);
}

void Entity::undo()
{
    hist_pos.pop_back();
    hist_alive.pop_back();
    pos = hist_pos.back();
    alive = hist_alive.back();
}

void Entity::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint)
{
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    tile.setTexture(&sprite);
    tile.setPosition(C0.x+delta*pos.x, C0.y+delta*pos.y);
    windowPoint->draw(tile);
}

void Entity::anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame)
{
    if (!alive) return;

    sf::Vector2i prev_pos = hist_pos.end()[-2];
    float deltaX = ((float)pos.x-(float)prev_pos.x)/4, deltaY = ((float)pos.y-(float)prev_pos.y)/4;
    float pxlX = C0.x + delta*(prev_pos.x+deltaX*frame), pxlY = C0.y + delta*(prev_pos.y+deltaY*frame);
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    tile.setTexture(&sprite);
    tile.setPosition(pxlX, pxlY);
    windowPoint->draw(tile);
}

//Methods for PlayerLike
PlayerLike::PlayerLike(){}

PlayerLike::PlayerLike(Json::Value json_entity, std::string name)
{
    pos = sf::Vector2i(json_entity["X"].asInt(), json_entity["Y"].asInt());
    hist_pos.push_back(pos);
    dir = json_entity["dir"].asCString()[0];
    hist_dir.push_back(dir);
    alive = json_entity["alive"].asBool();
    hist_alive.push_back(alive);

    char directions [] = {'U', 'R', 'D', 'L'};
    for (int i=0; i<4; i++)
    {
        sprites[directions[i]].resize(4);

        for (int j=0; j<4; j++)
        {
            sprite.loadFromFile(
                "assets/Entities/" + name + ".png",
                sf::IntRect(j*16, i*16, 16, 16)
            );
            sprites[directions[i]][j] = sprite;
        }

        sprite.loadFromFile(
            "assets/Entities/" + name + "_death.png",
            sf::IntRect(i*16, 0, 16, 16)
        );

        death_sprites[i] = sprite;
    }
}

sf::Vector2i PlayerLike::get_next_pos(char direction)
{
    if (direction == '_')
        direction = dir;
    
    return Entity::get_next_pos(direction);
}

sf::Vector2i PlayerLike::get_prev_pos()
{
    return hist_pos.back();
}

void PlayerLike::revert()
{
    if (dir == 'U')
        dir = 'D';
    else if (dir == 'R')
        dir = 'L';
    else if (dir == 'D')
        dir = 'U';
    else if (dir == 'L')
        dir = 'R';
}

void PlayerLike::validate_step()
{
    Entity::validate_step();
    hist_dir.push_back(dir);
}

void PlayerLike::undo()
{
    Entity::undo();
    hist_dir.pop_back();
    dir = hist_dir.back();
}

void PlayerLike::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint)
{
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    tile.setTexture(&sprites[dir][0]);
    tile.setPosition(C0.x+delta*pos.x, C0.y+delta*pos.y);
    windowPoint->draw(tile);
}

void PlayerLike::anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame)
{
    sf::Vector2i prev_pos = hist_pos.end()[-2];
    float deltaX = ((float)pos.x-(float)prev_pos.x)/4, deltaY = ((float)pos.y-(float)prev_pos.y)/4;
    float pxlX = C0.x + delta*(prev_pos.x+deltaX*frame), pxlY = C0.y + delta*(prev_pos.y+deltaY*frame);
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    tile.setTexture(&sprites[dir][step]);
    tile.setPosition(pxlX, pxlY);
    windowPoint->draw(tile);
}

void PlayerLike::destroy(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame)
{
    sf::Vector2i prev_pos = hist_pos.end()[-2];
    float pxlX = C0.x + delta * ((float)pos.x + (float)prev_pos.x) / 2;
    float pxlY = C0.y + delta * ((float)pos.y + (float)prev_pos.y) / 2;
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    tile.setTexture(&death_sprites[frame]);
    tile.setPosition(pxlX, pxlY);
    windowPoint->draw(tile);
}


//Methods for Boxes
Boxes::Boxes(){}

Boxes::Boxes(int nb_boxes, Json::Value boxes) : nb_boxes(nb_boxes)
{
    list.resize(nb_boxes);
    sf::Texture sprite;
    sprite.loadFromFile("assets/Entities/Box.png");

    for (int i=0; i<nb_boxes; i++)
    {
        sf::Vector2i pos(boxes[i]["X"].asInt(), boxes[i]["Y"].asInt());
        list[i] = Entity(pos, sprite);
    }
}

Entity* Boxes::get_box(sf::Vector2i coords, bool* hasBox)
{
    for (auto &box : list)
    {
        if (box.alive && box.pos == coords)
        {
            *hasBox = true;
            return &box;
        }
    }
    *hasBox = false;
    return &list[0];
}

std::vector<sf::Vector2i> Boxes::get_boxes_pos()
{
    std::vector<sf::Vector2i> boxes_pos;

    for (auto &box : list)
    {
        if (box.alive)
            boxes_pos.push_back(box.pos);
    }

    return boxes_pos;
}

void Boxes::destroy(std::vector<bool> to_destroy)
{
    int j = 0;
    for (int i = 0; i < nb_boxes; i++)
    {
        if (list[i].alive)
        {
            if (to_destroy[j])
            {
                list[i].alive = false;
            }
            j++;
        }
    }
}

void Boxes::validate_step()
{
    for (auto &box : list)
    {
        box.validate_step();
    } 
}

void Boxes::undo()
{
    for (auto &box : list)
    {
        box.undo();
    }
}

void Boxes::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint)
{
    for (auto &box : list)
    {
        if (box.alive)
            box.draw(C0, delta, windowPoint);
    }
}

void Boxes::anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame)
{
    for (auto &box : list)
    {
        box.anim(C0, delta, windowPoint, frame);
    }
}