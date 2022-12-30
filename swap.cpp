#include <iostream>
#include <list>

#include <SFML/Graphics.hpp>

#include "level.hpp"


int main()
{
    //SFML stuff and level initialization :
    sf::RenderWindow window(sf::VideoMode(500, 500), "SWAP!");
    sf::Font font;
    if (!font.loadFromFile("assets\\font.ttf"))
        std::cout << "Could not load font" << std::endl;

    int won = run(1, &window, font);
    window.close();

    if (won == 1)
    {
        std::cout << "level " << 1 << " won!" << std::endl;
    }
        
    return 0;
}