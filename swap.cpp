#include <SFML/Graphics.hpp>
#include "level.hpp"
#include <iostream>
#include <list>


Level level;
sf::RenderWindow window(sf::VideoMode(900, 460), "SWAP!" /*, sf::Style::Fullscreen*/);
sf::Clock sfclock;
sf::Font font;
std::string scene = "title";
const float SCREENW = window.getSize().x, SCREENH = window.getSize().y;

bool drawn = false;


void draw()
{
    window.clear(sf::Color(138, 208, 238));
    if (scene == "title")
    {
        sf::Texture title;
        title.loadFromFile("assets/title.png");
        float x = SCREENW/3, y = SCREENH/7;
        float w = x, h = w*title.getSize().y/title.getSize().x;
        sf::RectangleShape rect(sf::Vector2f(w, h));
        rect.setTexture(&title);
        rect.setPosition(x, y);
        window.draw(rect);

        float b_w = SCREENW/4, b_h = SCREENH/6;
        float b_x = SCREENW/2, b_y = 3*SCREENH/4;
        sf::RectangleShape button(sf::Vector2f(b_w, b_h));
        button.setOrigin(b_w/2, b_h/2);
        button.setPosition(b_x, b_y);
        button.setFillColor(sf::Color(0, 150, 0));
        window.draw(button);

        sf::Text button_text("Play", font, 2*b_h/3);
        sf::FloatRect textRect = button_text.getLocalBounds();
        button_text.setOrigin(textRect.left + textRect.width/2.0f,
                    textRect.top  + textRect.height/2.0f);
        button_text.setPosition(sf::Vector2f(b_x, b_y));
        window.draw(button_text);
    }
}

void draw_pause()
{
    level.display(&window, font);

    float b_w = SCREENW/4, b_h = SCREENH/5;
    float b_x = SCREENW/2, b_y = 3*SCREENH/10;
    sf::RectangleShape button(sf::Vector2f(b_w, b_h));
    button.setOrigin(b_w/2, b_h/2);
    button.setPosition(b_x, b_y);
    button.setFillColor(sf::Color(0, 150, 0));
    window.draw(button);

    sf::Text button_text("Continue", font, b_h/3);
    sf::FloatRect textRect = button_text.getLocalBounds();
    button_text.setOrigin(textRect.left + textRect.width/2.0f,
                textRect.top  + textRect.height/2.0f);
    button_text.setPosition(sf::Vector2f(b_x, b_y));
    window.draw(button_text);

    b_y = 7*SCREENH/10;
    button.setPosition(b_x, b_y);
    window.draw(button);

    button_text = sf::Text("Back to title", font, 2*b_h/3);
    textRect = button_text.getLocalBounds();
    button_text.setOrigin(textRect.left + textRect.width/2.0f,
                textRect.top  + textRect.height/2.0f);
    button_text.setPosition(sf::Vector2f(b_x, b_y));
    window.draw(button_text);
}

void click(sf::Vector2i mouse_pos)
{
    if (scene == "title")
    {
        float b_w = SCREENW/4, b_h = SCREENH/6;
        float b_x = SCREENW/2, b_y = 3*SCREENH/4;

        if (b_x-b_w/2 <= mouse_pos.x < b_x+b_w/2 && b_y-b_h/2 <= mouse_pos.y < b_y+b_h/2)
            scene = "level";
        level = Level();
        level.readfile(1);
    }

    else if (scene == "pause")
    {
        float b_w = SCREENW/4, b_h = SCREENH/5;
        float b_x = SCREENW/2, b_y = 3*SCREENH/10;

        if (b_x-b_w/2 <= mouse_pos.x < b_x+b_w/2 && b_y-b_h/2 <= mouse_pos.y < b_y+b_h/2)
            scene = "level";
    }
}

int main()
{
    if (!font.loadFromFile("assets\\font.ttf"))
        std::cout << "Could not load font" << std::endl;

    while (window.isOpen())
    {
        sf::Event evnt;
        if (window.pollEvent(evnt))
        {
            switch (evnt.type)
            {
                case sf::Event::Closed :
                    window.close();
                    break;
                
                case sf::Event::MouseButtonPressed :
                    if (evnt.mouseButton.button == sf::Mouse::Left)
                        click(sf::Vector2i(evnt.mouseButton.x, evnt.mouseButton.y));
                
                case sf::Event::Resized :
                    sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                    window.setView(sf::View(view));
                    level.resize_bg(&window);
                    drawn = false;
            }
        }

        if (scene == "pause" && !drawn)
        {
            std::cout << "in pause" << std::endl;
            draw_pause();
            window.display();
            drawn = true;
        }

        else if (scene == "level")
        {
            std::cout << "starting level" << std::endl;
            int result = level.run(&window, font, sfclock);
            if (result == 0)
                scene = "title";
            else if (result == 1)
                scene = "pause";
            drawn = false;
        }

        else if (!drawn)
        {
            std::cout << "in title" << std::endl;
            draw();
            window.display();
            drawn = true;
        }
    }

    return 0;
}