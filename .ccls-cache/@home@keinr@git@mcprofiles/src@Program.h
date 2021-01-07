#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED

#include <unordered_map>
#include <string>

#include <argh/argh.h>

#include "Profiles.h"

class Program {
    typedef argh::parser args_t;
    using comm_t = void(Program::*)(const args_t&);
    typedef std::unordered_map<std::string, comm_t> commands_t;

    Profiles profiles;
    commands_t commands;
    
    bool running;

    void loadProfile(const args_t &args);
    void delProfile(const args_t &args);
    void newProfile(const args_t &args);
    void listProfiles(const args_t &args);
    void closeProg(const args_t &args);

public:
    Program();
    void run();
};

#endif

