#include "UI/settings.hpp"

Option_Line::Option_Line() {}

Option_Line::Option_Line(sf::RenderWindow* win_p, std::string text, bool state, Alignment alignment) : text(text)
{
    ref_win_p = win_p;
    button = Toggle(alignment, win_p, state);
}

Options::Options() {}

Options::Options(Save* save_p, sf::RenderWindow* win_p)
{
    ref_save_p = save_p;
    ref_win_p = win_p;

    for (std::string key : save_p->get_flags())
    {
        Alignment alignment;    // To compute
        lines[key] = Option_Line(
            win_p,
            save_p->get_flag_desc(key),
            save_p->get_flag_state(key),
            alignment
        );
    }
}