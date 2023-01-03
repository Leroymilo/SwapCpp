#include "menu.hpp"
#include "button.hpp"

sf::Texture title;

Button start_;
Button exit_;


void draw_title(sf::RenderWindow* win_p, sf::Font font)
{
    win_p->clear(sf::Color(138, 208, 234));

    sf::RectangleShape title_rect;
    float W = title.getSize().x, H = title.getSize().y;
    title_rect.setSize(sf::Vector2f(0.5*W, 0.5*H));
    title_rect.setTexture(&title);
    float X = (win_p->getSize().x - title_rect.getSize().x) / 2;
    float Y = 0.1 * (win_p->getSize().y - title_rect.getSize().y);
    title_rect.setPosition(X, Y);
    win_p->draw(title_rect);

    start_.draw(font);
    exit_.draw(font);

    win_p->display();
}

int title_screen(sf::RenderWindow* win_p, sf::Font font)
{
    title.loadFromFile("assets/Menu/Title.png");
    start_ = Button("continue", "Start", Alignment(1, 0, 0, 7, 4, 0), win_p);
    exit_ = Button("exit", "Exit", Alignment(1, 0, 0, 7, 6, 0), win_p);

    // First draw
    draw_title(win_p, font);

    while (win_p->isOpen())
    {
        sf::Event evnt;
        if (win_p->pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
            {
                return 0;
            }
            
            else if (evnt.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                win_p->setView(sf::View(view));
                start_.reshape();
                exit_.reshape();
                draw_title(win_p, font);
                win_p->display();
            }
        }

        if (start_.clicked())
        {
            return 1;
        }
        else if (exit_.clicked())
        {
            return 0;
        }

        if (start_.update() || exit_.update())
        {
            draw_title(win_p, font);
        }
    }

    return 0;
}

int level_select(sf::RenderWindow* win_p)
{
    return 0;
}