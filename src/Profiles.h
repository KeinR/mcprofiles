#ifndef PROFILES_H_INCLUDED
#define PROFILES_H_INCLUDED

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <utility>
#include <filesystem>

class Profiles {
    using fs = std::filesystem;
    typedef std::unordered_map<std::string> data_t;
    typedef std::pair<std::size_t, std::size_t> playerofs_t;

    data_t data;
    fs::path path;
    std::string levelName;
   
    void read(const std::string &path);
    playerofs_t seek(std::ifstream &file);
    std::string getTimestamp();
    std::string snapshot();
public:
    Profiles(const std::string &path);
    void write();
    void load(const std::string &name);
    void remove(const std::string &name);
    void create(const std::string &name, const std::string &path);
    std::vector<std::string> getList();
    void add(const std::string &levelDataPath);
};

#endif

