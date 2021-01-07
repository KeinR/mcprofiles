#include <iostream>
#include <filesystem>

#include "Program.h"

namespace fs = std::filesystem;

int main(int argc, char **argv) {
    fs::current_path(fs::path(argv[0]).parent_path());

    try {
        Program prog;
        prog.run();
    } catch (std::exception &e) {
        std::cerr << "TERMINATED: " << e.what() << "\n";
    }
}


