
#include <SFML/Graphics.hpp>

sf::Texture title;


int draw_title(sf::RenderWindow* win_p)
{
    win_p->clear(sf::Color(138, 208, 234));

    

    return 1;
}

int menu(sf::RenderWindow* win_p)
{
    title.loadFromFile("assets/Menu/Title.png");

    while (win_p->isOpen())
    {

    }

    return 0;
}