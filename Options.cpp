#include "Pch.h"
#include "Options.h"

// ref : https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
const std::vector<std::string>& Options::ListLanguages()
{
    static std::vector<std::string> s_items;
    if (s_items.size() == 0)
    {
        s_items.push_back("en");
        s_items.push_back("ko");
    }
    return s_items;
}

void Options::IncreaseLanguage()
{
    auto&& list = ListLanguages();
    int d = std::distance(list.begin(), std::find(list.begin(), list.end(), language));
    language = list[(d + 1) % list.size()];
}

void Options::Save(const char* const fileName)
{
    std::ofstream ofs(fileName);
    
    Save(ofs);
}

void Options::Save(const filesystem::path& fileName)
{
    std::ofstream ofs(fileName);

    Save(ofs);
}

void Options::Save(std::ostream& os)
{
    os << "[options]" << std::endl
        << "resolutionX=" << resolution.first << std::endl
        << "resolutionY=" << resolution.second << std::endl
        << "language=" << language << std::endl;
}

void Options::Load(const char* const fileName)
{
    std::ifstream ifs(fileName);
    Load(ifs);
}

void Options::Load(const filesystem::path& fileName)
{
    std::ifstream ifs(fileName);
    Load(ifs);
}

void Options::Load(std::istream& s)
{
    std::vector<char> line(0x100, '\0');
    while (s.getline(line.data(), line.size()))
    {
        std::string s(line.data());
        int split = s.find_first_of('=');
        std::string key;
        std::string value;
        if (split == -1)
        {
            key = s;
        }
        else
        {
            key = s.substr(0, split);
            value = s.substr(split + 1);
        }

        if (key == "resolutionX")
        {
            resolution.first = std::stoi(value.c_str());
        }

        if (key == "resolutionY")
        {
            resolution.second = std::stoi(value.c_str());
        }

        if (key == "language")
        {
            language = value;
        }
        
    }
}