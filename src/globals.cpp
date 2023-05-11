#include <iostream>

#include "globals.hpp"

FontWrapper::FontWrapper() {};

sf::Font& FontWrapper::get_font()
{
    if (defined) return font;

    if (!font.loadFromFile("assets/font.ttf"))
        std::cout << "Could not load font" << std::endl;
    
    defined = true;
    return font;
}

int FontWrapper::size_text(std::string string, sf::Vector2f box_shape)
{
    unsigned int max_fs = 60, min_fs = 8;
    unsigned int font_size;

    sf::Text text;

    // YABS
    while (max_fs > min_fs) {
        font_size = (max_fs + min_fs + 1) / 2;
        text = sf::Text(string, font, font_size);
        sf::FloatRect bounds = text.getLocalBounds();
        
        if (bounds.width > box_shape.x || bounds.height > box_shape.y) {
            max_fs = font_size - 1;
        }

        else if (bounds.width < box_shape.x && bounds.height < box_shape.y) {
            min_fs = font_size;
        }

        else  {
            return font_size;
        }
    }

    return max_fs;
}