#ifndef SAVE_H
#define SAVE_H

#include <vector>
#include <map>
#include <string>

class Save
{
    private:
        int id;
        std::string dir;
        std::string name;
        std::map<int, std::pair<int, bool>> levels;
        int last_playable;

        std::string decode(std::string);
        std::string encode(std::string);
    
    public:
        Save(int);
        void refresh_playable();
        bool is_solved(int);
        bool is_perfected(int);
        bool is_playable(int);
        int get_steps(int);
        void solve(int lvl_id, int nb_steps, bool perf);
        void write();
};

#endif //SAVE_H