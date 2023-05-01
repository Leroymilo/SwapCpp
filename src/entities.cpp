#include <fstream>
#include <iostream>
#include <utility>

#include <json/value.h>
#include <json/json.h>

#include "gameplay/entities.hpp"


// Methods for BaseEntity
BaseEntity::BaseEntity(){}

BaseEntity::BaseEntity(Json::Value json_obj, std::string name)
{
    pos = sf::Vector2i(json_obj["X"].asInt(), json_obj["Y"].asInt());
    hist_pos.push_back(pos);
    alive = json_obj["alive"].asBool();
    hist_alive.push_back(alive);

    texture.loadFromFile("assets/Entities/" + name + ".png");
    texture.setRepeated(true);
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(DELTA * 4 * alive, 0, DELTA, DELTA));
}

sf::Vector2i BaseEntity::get_next_pos(Direction dir)
{
    if (dir == U)
        return sf::Vector2i(pos.x, pos.y-1);
    else if (dir == R)
        return sf::Vector2i(pos.x+1, pos.y);
    else if (dir == D)
        return sf::Vector2i(pos.x, pos.y+1);
    else if (dir == L)
        return sf::Vector2i(pos.x-1, pos.y);
    return pos;
}

void BaseEntity::validate_step()
{
    hist_pos.push_back(pos);
    hist_alive.push_back(alive);
}

void BaseEntity::undo()
// Cannot be called before validate_step
{
    hist_pos.pop_back();
    hist_alive.pop_back();
    pos = hist_pos.back();
    alive = hist_alive.back();
    set_sprite_rect(4, true);
}

void BaseEntity::set_sprite_rect(int anim_frame, bool force)
// Cannot be called before validate_step
{
    bool prev_alive = *(std::next(hist_alive.rbegin()));

    if (force || alive != prev_alive) {
        // The sprite needs to be updated
        int sprite_X = 4 * alive;   // default when forced
        if (prev_alive && !alive)
            sprite_X = 4 - anim_frame;  // breaking animation
        else if (!prev_alive && alive)  
            sprite_X = anim_frame;      // unused for now
        sprite.setTextureRect(sf::IntRect(sprite_X * DELTA, 0, DELTA, DELTA));
    }
}

void BaseEntity::draw(sf::Vector2i C0, int delta, sf::RenderWindow *windowPoint, int anim_frame)
// Cannot be called before validate_step
{
    set_sprite_rect(anim_frame);

    sf::Vector2i prev_pos = *(std::next(hist_pos.rbegin()));

    float deltaX = ((float)pos.x - (float)prev_pos.x) / 4;
    float deltaY = ((float)pos.y - (float)prev_pos.y) / 4;
    float pxlX = C0.x + delta * (prev_pos.x + deltaX * anim_frame);
    float pxlY = C0.y + delta * (prev_pos.y + deltaY * anim_frame);

    sprite.setTexture(texture);
    sprite.setPosition(pxlX, pxlY);
    sprite.setScale(delta/DELTA, delta/DELTA);
    windowPoint->draw(sprite);
}


// Methods for MovingEntity
MovingEntity::MovingEntity(){}

MovingEntity::MovingEntity(Json::Value json_obj, std::string name) : BaseEntity(json_obj, name)
{
    dir = CHAR2DIR.at(*json_obj["dir"].asCString());
    hist_dir.push_back(dir);

    sprite.setTextureRect(sf::IntRect(DELTA * 4 * alive, dir * DELTA, DELTA, DELTA));
}

sf::Vector2i MovingEntity::get_next_pos(Direction dir_)
{
    if (dir_ == _)
        dir_ = dir;
    
    return BaseEntity::get_next_pos(dir_);
}

sf::Vector2i MovingEntity::get_prev_pos()
// Cannot be called after validate_step
{
    return hist_pos.back();
}

void MovingEntity::revert()
{
    dir = static_cast<Direction>((dir + 2) % 4);
}

void MovingEntity::validate_step()
{
    BaseEntity::validate_step();
    hist_dir.push_back(dir);
}

void MovingEntity::undo()
// Cannot be called before validate_step
{
    BaseEntity::undo();
    hist_dir.pop_back();
    dir = hist_dir.back();
    set_sprite_rect(4, true);
}

void MovingEntity::set_sprite_rect(int anim_frame, bool force)
// Cannot be called before validate_step
{
    bool prev_alive = *(std::next(hist_alive.rbegin()));
    sf::Vector2i prev_pos = *(std::next(hist_pos.rbegin()));
    Direction prev_dir = *(std::next(hist_dir.rbegin()));
    
    if (force || (prev_alive != alive) || (prev_dir != dir) || (prev_pos != pos)) {
        // The sprite needs to be updated
        int sprite_X, sprite_Y = dir;

        // "death" animation
        if (prev_alive && !alive)
            sprite_X = 4 - anim_frame;
        else if (!prev_alive && alive)
            sprite_X = anim_frame;
        
        // changing step
        else if (alive)
        {
            step = (step + 1) % 4;
            sprite_X = 4 + step;
        }

        // dead
        else
            sprite_X = 0;
        
        sprite.setTextureRect(sf::IntRect(sprite_X * DELTA, sprite_Y * DELTA, DELTA, DELTA));
    }
}

void MovingEntity::draw(sf::Vector2i C0, int delta, sf::RenderWindow *windowPoint, int anim_frame)
// Cannot be called before validate_step
{
    set_sprite_rect(anim_frame);

    float pxlX = C0.x + pos.x * delta, pxlY = C0.y + pos.y * delta;

    // moving animation
    sf::Vector2i prev_pos = *(std::next(hist_pos.rbegin()));
    pxlX += delta * (4 - anim_frame) * (prev_pos.x - pos.x) / 4;
    pxlY += delta * (4 - anim_frame) * (prev_pos.y - pos.y) / 4;

    sprite.setTexture(texture);
    sprite.setPosition(pxlX, pxlY);
    sprite.setScale(delta/DELTA, delta/DELTA);
    windowPoint->draw(sprite);
}


// Methods for SwapperEntity
SwapperEntity::SwapperEntity() {}

SwapperEntity::SwapperEntity(Json::Value json_obj, std::string name, bool has_ghost) : has_ghost(has_ghost)
{
    physf = MovingEntity(json_obj["physf"], name + "_physf");
    ghost = MovingEntity(json_obj["ghost"], name + "_ghost");

    swap_texture.loadFromFile("assets/Entities/" + name + "_swap.png");
    swap_sprite_physf = sf::Sprite(swap_texture, sf::IntRect(0 * DELTA, physf.alive * DELTA, DELTA, DELTA));
    swap_sprite_ghost = sf::Sprite(swap_texture, sf::IntRect(4 * DELTA, ghost.alive * DELTA, DELTA, DELTA));
}

void SwapperEntity::swap()
{
    will_swap = true;

    sf::Vector2i temp_pos = physf.pos;
    physf.pos = ghost.pos;
    ghost.pos = temp_pos;

    Direction temp_dir = physf.dir;
    physf.dir = ghost.dir;
    ghost.dir = temp_dir;
    
    bool temp_alive = physf.alive;
    physf.alive = ghost.alive;
    ghost.alive = temp_alive;
}

void SwapperEntity::summon()
{
    ghost.alive = true;
    ghost.pos = physf.get_next_pos();
    ghost.dir = physf.dir;
}

void SwapperEntity::validate_step()
{
    physf.validate_step();
    ghost.validate_step();

    anim_swap = will_swap;
    will_swap = false;
}

void SwapperEntity::undo()
// Cannot be called before validate_step
{
    physf.undo();
    ghost.undo();
}

void SwapperEntity::set_swap_sprites_rect(int anim_frame)
// Cannot be called before validate_step
{
    swap_sprite_physf.setTextureRect(sf::IntRect(
        (anim_frame - 1) * DELTA,
        (physf.dir + 4 * physf.alive) * DELTA,
        DELTA, DELTA
    ));
    swap_sprite_ghost.setTextureRect(sf::IntRect(
        (3 - anim_frame) * DELTA,
        (ghost.dir + 4 * physf.alive) * DELTA,
        DELTA, DELTA
    ));
}

void SwapperEntity::draw(sf::Vector2i C0, int delta, sf::RenderWindow *windowP, int anim_frame)
{
    if (anim_swap && (anim_frame < 4)) {
        set_swap_sprites_rect(anim_frame);

        float pxlX, pxlY;

        pxlX = C0.x + physf.pos.x * delta, pxlY = C0.y + physf.pos.y * delta;
        swap_sprite_physf.setPosition(pxlX, pxlY);
        
        pxlX = C0.x + ghost.pos.x * delta, pxlY = C0.y + ghost.pos.y * delta;
        swap_sprite_ghost.setPosition(pxlX, pxlY);

        swap_sprite_physf.setTexture(swap_texture);
        swap_sprite_ghost.setTexture(swap_texture);
        swap_sprite_physf.setScale(delta/DELTA, delta/DELTA);
        swap_sprite_ghost.setScale(delta/DELTA, delta/DELTA);
        windowP->draw(swap_sprite_physf);
        windowP->draw(swap_sprite_ghost);
    }

    else {
        physf.draw(C0, delta, windowP, anim_frame);
        if (has_ghost) ghost.draw(C0, delta, windowP, anim_frame);
    }
}