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

    Json::Value levels_data;
    reader.parse(decode(actualJson["levels"].asCString()), levels_data);
    int nb_lvls = levels_data.size();

    for (int i=0; i < nb_lvls; i++)
    {
        int lvl_id = levels_data[i]["id"].asInt();
        int nb_steps = levels_data[i]["steps"].asInt();
        int perf = levels_data[i]["perf"].asBool();
        levels[lvl_id] = std::make_pair(nb_steps, perf);
    }

    for (int i=0; i < actualJson["flags"].size(); i++)
    {
        Json::Value flag = actualJson["flags"][i];
        std::string key = flag["key"].asCString();
        flags[key] = flag["value"].asBool();
        flags_descriptions[key] = flag["desc"].asCString();
    }

    refresh_playable();
}

Save Save::copy()
{
    this->write();
    return Save(id);
}

void Save::refresh_playable()
{
    // formula to change some day...
    int nb_unlocked = std::min(4, int(std::round(levels.size()/5)) + 1);

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
    return (levels.find(lvl_id) != levels.end());
}

bool Save::is_playable(int lvl_id)
{
    return (lvl_id <= last_playable || is_solved(lvl_id));
}

bool Save::is_perfected(int lvl_id)
{
    auto elt = levels.find(lvl_id);
    if (elt == levels.end()) return false;
    return elt->second.second;
}

int Save::get_steps(int lvl_id)
{
    return levels[lvl_id].first;
}

void Save::solve(int lvl_id, int nb_steps, bool perf)
{
    if (is_solved(lvl_id))
    {
        levels[lvl_id].first = std::min(levels[lvl_id].first, nb_steps);
        levels[lvl_id].second |= perf;
    }
    else
    {
        levels[lvl_id] = std::make_pair(nb_steps, perf);
        refresh_playable();
    }
}


std::vector<std::string> Save::get_flags()
{
    std::vector<std::string> keys;
    for (auto& elt : flags)
    {
        keys.push_back(elt.first);
    }
    return keys;
}

bool Save::get_flag_state(std::string key)
{
    if (flags.find(key) != flags.end())
    {
        return flags[key];
    }
    std::cout << "flag \"" << key << "\" not found in options" << std::endl;
    return false;
}

void Save::set_flag_state(std::string key, bool value)
{
    if (flags.find(key) != flags.end())
    {
        flags[key] = value;
        return;
    }
    std::cout << "flag \"" << key << "\" not found in options" << std::endl;
}


std::string Save::get_flag_desc(std::string key)
{
    if (flags_descriptions.find(key) != flags_descriptions.end())
    {
        return flags_descriptions[key];
    }
    std::cout << "flag \"" << key << "\" not found in options" << std::endl;
    return "";
}


void Save::write()
{
    Json::Value actualJson;

    Json::Value levels_data;
    for (auto& elt : levels)
    {
        Json::Value level;
        level["id"] = elt.first;
        level["steps"] = elt.second.first;
        level["perf"] = elt.second.second;
        levels_data.append(level);
    }

    Json::Value flags_data;
    for (auto& elt : flags)
    {
        Json::Value flag;
        flag["key"] = elt.first;
        flag["value"] = elt.second;
        flag["desc"] = flags_descriptions[elt.first];
        flags_data.append(flag);
    }

    Json::FastWriter fastWriter;

    actualJson.clear();
    actualJson["name"] = Json::Value(name);
    actualJson["levels"] = Json::Value(fastWriter.write(levels_data));
    actualJson["flags"] = flags_data;

    std::ofstream outfile(dir);
    outfile << actualJson;
    outfile.close();
}