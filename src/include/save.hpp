#ifndef SAVE_H
#define SAVE_H

#include <vector>
#include <map>
#include <string>

#include "globals.hpp"

class Save
{
    private:
        int id;
        std::string dir;
        std::string name;
        std::map<int, std::pair<int, bool>> levels;
        int last_playable;
        std::map<std::string, bool> flags;
        std::map<std::string, std::string> flags_descriptions;

        std::string decode(std::string);
        std::string encode(std::string);
    
    public:
        Save(int);
        Save copy();

        void refresh_playable();
        bool is_solved(int);
        int get_level_state(int);
        int get_steps(int);
        void solve(int lvl_id, int nb_steps, bool perf);

        std::vector<std::string> get_flags();
        bool get_flag_state(std::string);
        void set_flag_state(std::string, bool);
        std::string get_flag_desc(std::string);

        void write();
};

#endif //SAVE_H