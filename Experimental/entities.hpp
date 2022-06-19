#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>

class Entity    //Generic class describing entities
{
    protected:
        sf::Texture sprite;
        
    public:
        sf::Vector2i C;

        Entity();
        Entity(sf::Vector2i C, sf::Texture sprite);
        sf::Vector2i getNextPos(char direction);
        void draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint);
        void anim(sf::Vector2i start, sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame);

};

class PlayerLike : public Entity   //Everything that can wove on its own (Player, Bullet, Monsters)
{
    private:
        std::map<char, sf::Texture> sprites;

    public:
        char direction;

        PlayerLike();
        PlayerLike(std::string directory, std::string name);
        sf::Vector2i getNextPos(char direction = '_');
        void revert();
        void draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint);
        void anim(sf::Vector2i start, sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame);
};

class Boxes     //list of boxes (simple entities)
{
    private:
        int nbBoxes;
        Entity * list;
        bool * listAlive;
    
    public:
        Boxes();
        Boxes(const Boxes& tocopy);
        Boxes& operator=(const Boxes& other);
        ~Boxes();
        void readFile(std::string directory);
        void setLists(int n, Entity * list, bool * listAlive);

        Entity* getBox(sf::Vector2i coords, bool* hasBox);
        std::vector<sf::Vector2i> get_boxes_pos();

        void draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint);
        void anim(Boxes prevState, sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint, int frame);
};

#endif //ENTITY_H