#include "UI/settings.hpp"

Option_Line::Option_Line() {}

Option_Line::Option_Line(sf::RenderWindow* win_p, std::string text, bool state, Alignment alignment) : text(text)
{
    ref_win_p = win_p;
    button = Toggle(alignment, win_p, state);
}

void Option_Line::reshape()
{
    
}



Options::Options() {}

Options::Options(Save* save_p, sf::RenderWindow* win_p, sf::Font font)
{
    ref_save_p = save_p;
    ref_win_p = win_p;
    std::vector<std::string> flags = save_p->get_flags();

    // finding the longest text to align all buttons :
    float max_text_W = 0;
    for (std::string key : flags)
    {
        sf::Text desc(save_p->get_flag_desc(key), font, 20);
        float width = desc.getLocalBounds().width;
        if (width > max_text_W)
        {
            max_text_W = width;
        }
    }

    // initializing lines :
    for (int i = 0; i < flags.size(); i++)
    {
        Alignment align(2, 1, max_text_W, flags.size(), i, 16);
        lines[flags[i]] = Option_Line(
            win_p,
            save_p->get_flag_desc(flags[i]),
            save_p->get_flag_state(flags[i]),
            align
        );
    }
}