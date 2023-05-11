#ifndef GLOBALS_H
#define GLOBALS_H

#include <map>

#include <SFML/Graphics.hpp>

#define __min(a,b) (((a) < (b)) ? (a) : (b))
#define __max(a,b) (((a) > (b)) ? (a) : (b))

// sf::Vector2i Comparator to use sf::Vector2i as a std::map key
// Found here : https://stackoverflow.com/questions/56506121/sfvector2i-as-key-in-stdmap-shows-as-1-1-if-the-y-value-is-1-even-if-the
struct VectorComparator
{
    bool operator() (sf::Vector2i lhs, sf::Vector2i rhs) const
    {
        return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
    }
};

// sf::Vector2i Hahser to store sf::Vector2i in a std::unordered_set or use it as a std::unordered_map key
// Made by me :)
struct VectorHasher
{
    std::size_t operator()(const sf::Vector2i &V) const {
        return abs(V.x ^ V.y);
    }
};

static int const DELTA = 16;

enum Direction {U, R, D, L, _};
static std::map<char, Direction> const CHAR2DIR = {{'U', U}, {'R', R}, {'D', D}, {'L', L}};
static std::map<Direction, char> const DIR2CHAR = {{U, 'U'}, {R, 'R'}, {D, 'D'}, {L, 'L'}};


// For the same font to be accessible by every header and cpp file :
class FontWrapper
{
    private:
        sf::Font font;
        bool defined = false;
    public:
        FontWrapper();
        sf::Font &get_font();
        int size_text(std::string, sf::Vector2f);
};

static FontWrapper font;

#endif //GLOBALS_H