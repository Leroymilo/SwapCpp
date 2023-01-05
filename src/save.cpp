#include <iomanip>
#include <fstream>

#include <json/value.h>
#include <json/json.h>

#include "UI/save.hpp"

Save::Save(int save_nb)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << save_nb;

    std::ifstream file("saves/" + ss.str() + ".json");
    Json::Value actualJson;
    Json::Reader reader;
    
    reader.parse(file, actualJson);

    name = actualJson["name"].asCString();

    std::string data = decode(actualJson["data"].asCString());

    reader.parse(data, actualJson);

    
}

std::string Save::encode(std::string string)
{
    return string;
}

std::string Save::decode(std::string string)
{
    return string;
}