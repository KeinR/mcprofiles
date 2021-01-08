#ifndef PROFILES_H_INCLUDED
#define PROFILES_H_INCLUDED

#include <string>
#include <vector>
#include <fstream>
#include <utility>
#include <filesystem>

class Profiles {

    typedef std::pair<std::size_t, std::size_t> playerofs_t;

    std::filesystem::path path;
    std::string levelName;
   
    playerofs_t seek(std::ifstream &file);
    std::string getTimestamp();
    std::string snapshot();
    void backup();
    std::vector<char> loadFile(std::ifstream &file);
    void save(const std::string &name);
    void init();
public:
    Profiles(const std::string &path);
    void load(const std::string &name);
    void remove(const std::string &name);
    std::vector<std::string> getList();
};

#endif

