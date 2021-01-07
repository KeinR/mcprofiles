#include "Program.h"

#include <iostream>

#include <cpp-terminal/terminal.h>

#include "util.h"


Program::Program(): profiles("profiles"), running(running) {
}

void Program::loadProfile(const args_t &args) {
    profiles.load(args[1]);
}

void Program::delProfile(const args_t &args) {
    profiles.remove(args[1]);
}

void Program::newProfile(const args_t &args) {
    profiles.create(args[1], "foo");
}

void Program::listProfiles(const args_t &args) {
    std::vector<std::string> pfps = profiles.getList();
    for (std::string &str : pfps) {
        std::cout << str << '\n';
    }
}

void Program::closeProg(const args_t &args) {
    running = false;
}

void Program::run() {
    while (running) {
        using Term::color;
        using Term::fg;

        std::string buffer;
        std::cin >> buffer; 

        std::vector<std::string> params = ut::tokenize(buffer);
        std::vector<char*> cparams;
        cparams.reserve(params.size());
        std::transform(params.begin(), params.end(), std::back_inserter(cparams), [](std::string &str) -> char* {
            return str.data();
        });
        args_t args(cparams.size(), cparams.data());

        commands_t::iterator it = commands.find(args[0]);
        if (it != commands.end()) {
            (this->*(it->second))(args);
        } else {
            std::cout << color(fg::red) << "Error" << color(fg::reset) << ": unknown command \"" << args[0] << "\"\n";
        }

    }
}

