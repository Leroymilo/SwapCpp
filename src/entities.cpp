#include <fstream>
#include <iostream>

#include <json/value.h>
#include <json/json.h>

#include "gameplay/entities.hpp"


//Methods for Entity
Entity::Entity(){}


Entity::Entity(sf::Vector2i C, sf::Texture sprite) : C(C), prev_C(C), sprite(sprite)
{
    is_alive = true;
    prev_is_alive = true;
}

sf::Vector2i Entity::get_next_pos(char direction)
{
    if (direction == 'U')
        return sf::Vector2i(C.x, C.y-1);
    else if (direction == 'R')
        return sf::Vector2i(C.x+1, C.y);
    else if (direction == 'D')
        return sf::Vector2i(C.x, C.y+1);
    else if (direction == 'L')
        return sf::Vector2i(C.x-1, C.y);
    return C;
}

sf::Vector2i Entity::get_prev_pos(char direction)
{    
    if (direction == 'U')
        return sf::Vector2i(C.x, C.y+1);
    else if (direction == 'R')
        return sf::Vector2i(C.x-1, C.y);
    else if (direction == 'D')
        return sf::Vector2i(C.x, C.y-1);
    else if (direction == 'L')
        return sf::Vector2i(C.x+1, C.y);
    return C;
}

void Entity::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint)
{
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    tile.setTexture(&sprite);
    tile.setPosition(C0.x+delta*C.x, C0.y+delta*C.y);
    windowPoint->draw(tile);
}

void Entity::anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame)
{
    float deltaX = ((float)C.x-(float)prev_C.x)/4, deltaY = ((float)C.y-(float)prev_C.y)/4;
    float pxlX = C0.x + delta*(prev_C.x+deltaX*frame), pxlY = C0.y + delta*(prev_C.y+deltaY*frame);
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    tile.setTexture(&sprite);
    tile.setPosition(pxlX, pxlY);
    windowPoint->draw(tile);
}

//Methods for PlayerLike
PlayerLike::PlayerLike(){}

PlayerLike::PlayerLike(Json::Value json_entity, std::string name)
{
    C = sf::Vector2i(json_entity["X"].asInt(), json_entity["Y"].asInt());
    prev_Cs.push_back(C);
    dir = json_entity["dir"].asCString()[0];

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
    
    is_alive = json_entity["alive"].asBool();
    prev_is_alive = is_alive;
}

sf::Vector2i PlayerLike::get_next_pos(char direction)
{
    if (direction == '_')
        direction = dir;
    
    if (direction == 'U')
        return sf::Vector2i(C.x, C.y-1);
    else if (direction == 'R')
        return sf::Vector2i(C.x+1, C.y);
    else if (direction == 'D')
        return sf::Vector2i(C.x, C.y+1);
    else if (direction == 'L')
        return sf::Vector2i(C.x-1, C.y);
    return C;
}

sf::Vector2i PlayerLike::get_prev_pos(char direction)
{
    if (direction == '_')
        direction = dir;
    
    if (direction == 'U')
        return sf::Vector2i(C.x, C.y+1);
    else if (direction == 'R')
        return sf::Vector2i(C.x-1, C.y);
    else if (direction == 'D')
        return sf::Vector2i(C.x, C.y-1);
    else if (direction == 'L')
        return sf::Vector2i(C.x+1, C.y);
    return C;
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

void PlayerLike::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint)
{
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    tile.setTexture(&sprites[dir][0]);
    tile.setPosition(C0.x+delta*C.x, C0.y+delta*C.y);
    windowPoint->draw(tile);
}

void PlayerLike::anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame)
{
    float deltaX = ((float)C.x-(float)prev_Cs.back().x)/4, deltaY = ((float)C.y-(float)prev_Cs.back().y)/4;
    float pxlX = C0.x + delta*(prev_Cs.back().x+deltaX*frame), pxlY = C0.y + delta*(prev_Cs.back().y+deltaY*frame);
    sf::RectangleShape tile(sf::Vector2f(delta, delta));
    tile.setTexture(&sprites[dir][frame]);
    tile.setPosition(pxlX, pxlY);
    windowPoint->draw(tile);
}

void PlayerLike::destroy(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame)
{
    float pxlX = C0.x + delta * ((float)C.x + (float)prev_Cs.back().x) / 2;
    float pxlY = C0.y + delta * ((float)C.y + (float)prev_Cs.back().y) / 2;
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
        sf::Vector2i C(boxes[i]["X"].asInt(), boxes[i]["Y"].asInt());
        list[i] = Entity(C, sprite);
    }
}

Entity* Boxes::get_box(sf::Vector2i coords, bool* hasBox)
{
    for (auto &box : list)
    {
        if (box.is_alive && box.C == coords)
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
        if (box.is_alive)
            boxes_pos.push_back(box.C);
    }

    return boxes_pos;
}

void Boxes::destroy(std::vector<bool> to_destroy)
{
    int j = 0;
    for (int i = 0; i < nb_boxes; i++)
    {
        if (list[i].is_alive)
        {
            if (to_destroy[j])
            {
                list[i].is_alive = false;
            }
            j++;
        }
    }
}

void Boxes::step_end_logic()
{
    for (auto &box : list)
    {
        box.prev_is_alive = box.is_alive;
        box.prev_C = box.C;
        if (!box.is_alive)
            box.step_since_destroy++;
    } 
}

void Boxes::undo()
{
    for (auto &box : list)
    {
        if (!box.is_alive)
        {
            box.step_since_destroy--;
            if (box.step_since_destroy == 0)
                box.is_alive = true;
            else
                box.step_since_destroy--;
        }
    }
}

void Boxes::draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint)
{
    for (auto &box : list)
    {
        if (box.is_alive)
            box.draw(C0, delta, windowPoint);
    }
}

void Boxes::anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame)
{
    for (auto &box : list)
    {
        if (box.is_alive && box.prev_is_alive)
            box.anim(C0, delta, windowPoint, frame);
        else if (box.is_alive)
            box.draw(C0, delta, windowPoint);
        
    }
}