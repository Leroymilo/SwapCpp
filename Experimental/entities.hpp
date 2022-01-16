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

};

class PlayerLike : public Entity   //Everything that can wove on its own (Player, Bullet, Monsters)
{
    private:
        std::map<char, sf::Texture> sprites;

    public:
        sf::Vector2i C;
        char direction;

        PlayerLike();
        PlayerLike(std::string directory, std::string name);
        sf::Vector2i getNextPos();
        void draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint);
};

class Boxes     //list of boxes (simple entities)
{
    private:
        int nbBoxes;
        Entity * list;
        bool * listAlive;
    
    public:
        Boxes();
        void readFile(std::string directory);
        Entity* getBox(sf::Vector2i coords, bool* hasBox);
        void draw(sf::Vector2i C0, int delta, sf::RenderWindow* windowPoint);
};

#endif //ENTITY_H