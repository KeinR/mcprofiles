#ifndef PROFILES_H_INCLUDED
#define PROFILES_H_INCLUDED

#include <string>
#include <vector>
#include <fstream>
#include <utility>
#include <filesystem>
#include <istream>

class Profiles {

    typedef std::string::const_iterator str_iterator_t;
    typedef std::pair<str_iterator_t, str_iterator_t> playerofs_t;

    std::filesystem::path path;
    std::string levelName;
    int dbTreeLevel;
    bool debug;
    bool verbose;
   
    playerofs_t seek(const std::string &data);
    std::string getTimestamp();
    long getTimestampLong();
    std::string snapshot();
    void backup();
    std::string loadFromFile(std::istream &file);
    std::string loadFile(const std::filesystem::path &p);
    void writeFile(const std::filesystem::path &p, const std::string &data);
    void save(const std::string &name);
    void init();
    void createDir(const std::filesystem::path &path);
    bool confirm();
    std::string loadNBT(const std::string &path, bool &failed);
    std::string loadNBT(const std::filesystem::path &path, bool &failed);
    void writeNBT(const std::filesystem::path &path, const std::string &data, bool &failed);
    void writeNBT(const std::string &path, const std::string &data, bool &failed);
    unsigned short readShort(const str_iterator_t &it);
    int readInt(const str_iterator_t &it);
    void parseList(str_iterator_t &it, const str_iterator_t &end);
    void parseCompound(str_iterator_t &it, const str_iterator_t &end);
    void parseUNTag(str_iterator_t &it, const str_iterator_t &end, int type);
    void levelFileCorrupted();
    void tree(const std::string &str);
    void status(const std::string &str);
    void log(const std::string &str);
    void bindump(const std::string &data);
public:
    Profiles(const std::string &path, bool debug, bool verbose);
    void load(const std::string &name);
    void remove(const std::string &name);
    std::vector<std::string> getList();
};

#endif

