#ifndef SAVE_H
#define SAVE_H

#include <vector>
#include <map>

class Save
{
    private:
        std::vector<int> solved;
        std::map<int, int> steps;
        int last_playable;
        std::string name;
        std::string decode(std::string);
        std::string encode(std::string);
    
    public:
        Save(int);
        bool is_solved(int);
        bool is_playable(int);
};

#endif //SAVE_H