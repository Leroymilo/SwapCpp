#include <iostream>

#include "globals.hpp"

CFont::CFont() {};

sf::Font& CFont::get_font()
{
    if (defined) return font;

    if (!font.loadFromFile("assets/font.ttf"))
        std::cout << "Could not load font" << std::endl;
    
    defined = true;
    return font;
}