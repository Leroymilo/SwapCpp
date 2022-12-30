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

    int res = run(1, &window, font);

    if (res)
        window.close();

    return 0;
}