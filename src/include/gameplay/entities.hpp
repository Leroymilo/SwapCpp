#ifndef ENTITY_H
#define ENTITY_H

#include <list>

#include <SFML/Graphics.hpp>
#include <json/value.h>

class Entity    //Generic class describing entities
{
    protected:
        sf::Texture sprite;
        std::vector<bool> hist_alive;
        std::vector<sf::Vector2i> hist_pos;
        
    public:
        bool alive;
        sf::Vector2i pos;

        Entity();
        Entity(sf::Vector2i pos, sf::Texture sprite);
        sf::Vector2i get_next_pos(char direction);
        void validate_step();
        void undo();
        void draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint);
        void anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame);

};

class PlayerLike : public Entity   //Everything that can wove on its own (Player and Ghost for now)
{
    private:
        std::map<char, std::vector<sf::Texture>> sprites;
        sf::Texture death_sprites [4];
        std::vector<char> hist_dir;

    public:
        char dir;
        int step = 0;

        PlayerLike();
        PlayerLike(Json::Value json_entity, std::string name);
        sf::Vector2i get_next_pos(char direction = '_');
        sf::Vector2i get_prev_pos();
        void revert();
        void validate_step();
        void undo();
        void draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint);
        void anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame);
        void destroy(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame);
};

class Boxes     //list of boxes (simple entities)
{
    private:
        int nb_boxes;
        std::vector<Entity> list;
    
    public:
        Boxes();
        Boxes(int nb_boxes, Json::Value boxes);

        Entity* get_box(sf::Vector2i coords, bool* hasBox);
        std::vector<sf::Vector2i> get_boxes_pos();
        void destroy(std::vector<bool> to_destroy);
        void validate_step();
        void undo();

        void draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint);
        void anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame);
};

#endif //ENTITY_H