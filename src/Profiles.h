#ifndef PROFILES_H_INCLUDED
#define PROFILES_H_INCLUDED

#include <string>
#include <vector>
#include <utility>

#include "Profile.h"

class Profiles {
    std::vector<Profile> data;
    
    void read(const std::string &path);
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

