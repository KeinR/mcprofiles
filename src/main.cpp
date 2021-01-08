#include <iostream>
#include <filesystem>
#include <string>
#include <stdexcept>
#include <exception>

#include <argh/argh.h>

#include "Profiles.h"

#define EXIT_ERROR 1
#define EXIT_OK 0

void printUsage(const char *execName);
std::string getUser(argh::parser &com);

int main(int argc, char **argv) {

    namespace fs = std::filesystem;

    argh::parser com(argc, argv);

    if (com({"-h", "--help"})) {
        return EXIT_OK;
    }

    if (argc < 3) {
        printUsage(argv[0]);
        return EXIT_ERROR;
    }

    std::string command = com[1];
    std::string target = com[com.size() - 1];

    if (!fs::exists(target) || !fs::is_directory(target)) {
        std::cerr << "Error: target does not exist or is not a directory\n";
        return EXIT_ERROR;
    }

    Profiles pfp(target);

    try {

        if (command == "remove") {
            pfp.remove(getUser(com)); 
        } else if (command == "load") {
            pfp.load(getUser(com));
        } else if (command == "list") {
            for (std::string &str : pfp.getList()) {
                std::cout << str << '\n';
            }
        } else {
            std::cerr << "Error: unknown command \"" << command << "\"\nUse '--help' for usage\n";
            return EXIT_ERROR;
        }

    } catch (std::exception &e) {
        std::cerr << "Error: command " << command << " failed: " << e.what() << '\n';
    }

    return EXIT_OK;
}

void printUsage(const char *execName) {
    std::cout << "Usage\n" << execName <<
R"( command [options] target
Options

    command                 The command to invoke on the target
                            Can be one of:
                                - remove - remove the specified profile
                                - load - load the specified profile.
                                         If it doesn't exist, it is created from the current one.
                                - list - list the current profiles
    target                  The target save directory

    -u|--user  [string]     The specified profile name

    -h|--help               Print this message
    
)";
}

std::string getUser(argh::parser &com) {
    std::string user;
    com({"-u", "--user"}) >> user;
    if (!user.size()) {
        throw std::invalid_argument("Must supply user");
    }
    return user;
}

