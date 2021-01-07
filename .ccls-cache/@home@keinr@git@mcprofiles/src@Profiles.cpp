#include "Profiles.h"

#include <filesystem>
#include <algorithm>

#include <nbt/stream_reader.h>

namespace fs = std::filesystem;

Profiles::Profiles(const std::string &path) {
    for (fs::directory_entry e : fs::directory_iterator()) {
        load(e.path().string());
    }
}

void Profiles::read(const std::string &path) {
    
}

void Profiles::write() {

}

void Profiles::load(const std::string &name) {

}

void Profiles::remove(const std::string &name) {

}

void Profiles::create(const std::string &name, const std::string &path) {

}

std::vector<std::string> Profiles::getList() {
    std::vector<std::string> result;
    result.reserve(data.size());
    std::transform(data.begin(), data.end(), std::back_inserter(result), [](Profile &p) -> std::string {
        return p.name;
    });
    return result;
}


