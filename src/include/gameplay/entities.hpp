#ifndef ENTITY_H
#define ENTITY_H

#include <list>

#include <SFML/Graphics.hpp>
#include <json/value.h>

#include "globals.hpp"

class BaseEntity
// Generic class describing entities,
// Contains coordinates, aliveness, step validation, undoing and drawing function
// The texture is a spritesheet with sprites for clean, broken and a breaking animation
{
    protected:
        sf::Texture texture;
        sf::Sprite sprite;
        std::list<bool> hist_alive;
        std::list<sf::Vector2i> hist_pos;

        void set_sprite_rect(int, bool force = false);
        
    public:
        bool alive;
        sf::Vector2i pos;

        BaseEntity();
        BaseEntity(Json::Value, std::string);

        sf::Vector2i get_next_pos(Direction);

        void validate_step();
        void undo();
        void draw(sf::Vector2i C0, int delta, sf::RenderWindow*, int);
};

class MovingEntity : public BaseEntity
// Any entity that moves on its own, the distinction between ghost and physf is done in the level itself.
// Inherits from BaseEntity and contains orientation as well as a method to reverse its orientation
// The spreadsheet has sprites for oriented alive (with step cycle) and oriented death animation
{
    private:
        std::list<Direction> hist_dir;

        void set_sprite_rect(int, bool force = false);

    public:
        Direction dir;
        int step = 0;

        MovingEntity();
        MovingEntity(Json::Value, std::string);

        sf::Vector2i get_next_pos(Direction = _);
        sf::Vector2i get_prev_pos();
        void revert();

        void validate_step();
        void undo();
        void draw(sf::Vector2i C0, int delta, sf::RenderWindow*, int);
        // It's the same as its parent class draw() except it needs to calls its own set_sprite_rect()
};

class SwapperEntity
// An entity able to swap between a ghost and a physf (short for physical form),
// both can be "alive" at the same time and are separate MovingEntities.
// Contains an additional texture and sprite for a swapping animation, and a swap function
{
    private:
        sf::Texture swap_texture;
        sf::Sprite swap_sprite_physf;
        sf::Sprite swap_sprite_ghost;

        bool has_ghost;

        bool will_swap = false, anim_swap = false;

        void set_swap_sprites_rect(int);

    public:
        MovingEntity physf;
        MovingEntity ghost;

        SwapperEntity();
        SwapperEntity(Json::Value, std::string, bool has_ghost);

        void swap();
        void summon();

        void validate_step();
        void undo();
        void draw(sf::Vector2i C0, int delta, sf::RenderWindow*, int);
};

#endif //ENTITY_H