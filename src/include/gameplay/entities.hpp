#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include <list>

class Entity    //Generic class describing entities
{
    protected:
        sf::Texture sprite;
        
    public:
        bool is_alive;
        bool prev_is_alive;
        int step_since_destroy = 0;
        sf::Vector2i C;
        sf::Vector2i prev_C;

        Entity();
        Entity(sf::Vector2i C, sf::Texture sprite);
        sf::Vector2i get_next_pos(char direction);
        sf::Vector2i get_prev_pos(char direction);
        void draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint);
        void anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame);

};

class PlayerLike : public Entity   //Everything that can wove on its own (Player, Bullet, Monsters)
{
    private:
        std::map<char, sf::Texture> sprites;

    public:
        char dir;
        std::list<sf::Vector2i> prev_Cs;

        PlayerLike();
        PlayerLike(std::string directory, std::string name);
        sf::Vector2i get_next_pos(char direction = '_');
        sf::Vector2i get_prev_pos(char direction = '_');
        void revert();
        void reset_dir();
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
        Boxes(std::string directory);

        Entity* get_box(sf::Vector2i coords, bool* hasBox);
        std::vector<sf::Vector2i> get_boxes_pos();
        void destroy(std::vector<bool> to_destroy);
        void step_end_logic();
        void undo();

        void draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint);
        void anim(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame);
};

#endif //ENTITY_H