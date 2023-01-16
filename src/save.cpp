#include <iomanip>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <filesystem>

#include <json/value.h>
#include <json/json.h>

#include "save.hpp"


std::string Save::encode(std::string string)
{
    return string;
}

std::string Save::decode(std::string string)
{
    return string;
}

Save::Save(int save_nb) : id(save_nb)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << save_nb;

    if (!std::filesystem::exists("saves"))
    {
        std::filesystem::create_directory("saves");
    }

    dir = "saves/save" + ss.str() + ".json";

    if (!std::filesystem::exists(dir))
    {
        std::filesystem::copy_file("saves/blank_save.json", dir);
    }

    std::ifstream file(dir);
    Json::Value actualJson;
    Json::Reader reader;
    
    reader.parse(file, actualJson);

    name = actualJson["name"].asCString();

    std::string data = decode(actualJson["data"].asCString());

    reader.parse(data, actualJson);
    int nb_lvls = actualJson["nb_comp"].asInt();

    for (int i=0; i < nb_lvls; i++)
    {
        int lvl_id = actualJson["lvl_ids"][i].asInt();
        int nb_steps = actualJson["steps"][i].asInt();
        solved.push_back(lvl_id);
        steps[lvl_id] = nb_steps;
    }

    refresh_playable();
}

void Save::refresh_playable()
{
    int nb_unlocked = std::min(4, int(std::round(solved.size()/5)) + 1);

    int i = 1;
    while (nb_unlocked > 0)
    {
        if (!is_solved(i))
        {
            nb_unlocked--;
        }

        i++;
    }

    last_playable = i - 1;
}

bool Save::is_solved(int lvl_id)
{
    return (std::find(solved.begin(), solved.end(), lvl_id) != solved.end());
}

bool Save::is_playable(int lvl_id)
{
    return (lvl_id <= last_playable || is_solved(lvl_id));
}

void Save::solve(int lvl_id, int nb_steps)
{
    if (is_solved(lvl_id))
    {
        steps[lvl_id] = std::min(steps[lvl_id], nb_steps);
    }
    else
    {
        solved.push_back(lvl_id);
        steps[lvl_id] = nb_steps;
    }

    refresh_playable();
}

void Save::write()
{
    Json::Value actualJson;

    actualJson["nb_comp"] = Json::Value(solved.size());

    Json::Value lvl_ids;
    Json::Value steps;
    for (auto& elt : this->steps)
    {
        lvl_ids.append(Json::Value(elt.first));
        steps.append(Json::Value(elt.second));
    }
    actualJson["lvl_ids"] = lvl_ids;
    actualJson["steps"] = steps;

    Json::FastWriter fastWriter;
    std::string data = fastWriter.write(actualJson);

    actualJson.clear();
    actualJson["name"] = Json::Value(name);
    actualJson["data"] = Json::Value(data);

    std::ofstream outfile(dir);
    outfile << actualJson;
    outfile.close();
}