#include "Profiles.h"

#include <filesystem>
#include <algorithm>
#include <limits>

namespace fs = std::filesystem;

#define SIGNATURE ".mcprofiles"
#define BACKUP "backup"
#define USERS "users"
#define MAX_BACKUPS 20
#define LEVEL_FILE "level.dat"

#define TAG_End 0x00
#define TAG_Compound 0x0A
#define TAG_Byte 0x01

Profiles::Profiles(const std::string &path) {
    for (fs::directory_entry e : fs::directory_iterator()) {
        load(e.path().string());
    }
}

std::string Profiles::getTimestamp() {
    auto time = std::chrono::system_clock::now();
    long epoch = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
    return std::to_string(epoch);
}

Profiles::playerofs_t Profiles::seek(std::ifstream &file) {

    const std::string tagName = "Player";
    
    playerofs_t player;

    while (file.good()) {
        if (file.get() == TAG_Compound) {
            short length;
            file >> length;
            if (length == tagName.size()) {
                std::string buffer(tagName.size(), '.');
                file.read(buffer.data(), buffer.size());
                if (tagName == buffer) {
                    player.first = file.tellg();
                    break; // Tag found
                }
            }
        }
    }

    std::string tag2Name = "raining";

    file.seekg(-1, file.end);
    int bytes = 3;
    while (file.good()) {
        file.seekg(-1, file.cur);
        if (file.peek() == TAG_Byte && --bytes <= 0) {
            std::size_t pos = file.tellg();
            file.ignore(1);
            short length;
            file >> length;
            if (length == tag2Name.size()) {
                std::string buffer(tag2Name.data(), tag2Name.size());
                file.read(buffer.data(), buffer.size());
                if (tag2Name == buffer) {
                    player.second = file.tellg();
                    break;
                }
            }
            
            file.seekg(pos, file.beg);
        }
    }


    return player;
}

std::string Profiles::snapshot() {

    std::ifstream file;

    file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    file.open(path / LEVEL_FILE, std::ifstream::binary);

    seek(file);

    playerofs_t pos = seek(file);

    std::string buffer('.', pos.second - pos.first);

    file.seekg(pos.first, file.beg);
    file.read(buffer.data(), buffer.size());

    return buffer;
}

void Profiles::backup() {
    fs::path level = path / LEVEL_FILE;
    fs::path backupDir = path / SIGNATURE / BACKUP; 
    fs::create_directories(backupDir);
    fs::path backup = backupDir / getTimestamp();
    fs::copy_file(level, backup);

    fs::path oldest = backup;
    int count = 0;
    long oldestTime = std::numeric_limits<long>::max();
    for (fs::directory_entry e : fs::directory_iterator(backupDir)) {
        try {
            fs::path p = e.path();
            long time = std::stol(p.filename().string());
            if (time < oldestTime) {
                oldestTime = time;
                oldest = p;
            }
            count++;
        } catch (std::exception &e) {
            // Invalid file name
        }
    }
}

void Profiles::read(const std::string &path) {

}

void Profiles::write() {

}

void Profiles::load(const std::string &name) {
    data_t::iterator it = data.find(name);
    if (it != data.end()) {
        create(name);
    }
}

void Profiles::remove(const std::string &name) {

}

void Profiles::open(const std::string &name) {
    fs::path dir = path / SIGNATURE / USERS;
    fs::create_directories(dir);
    std::ifstream file(name);
    nbt::stream_reader stream();
}

std::vector<std::string> Profiles::getList() {
    std::vector<std::string> result;
    result.reserve(data.size());
    std::transform(data.begin(), data.end(), std::back_inserter(result), [](Profile &p) -> std::string {
        return p.name;
    });
    return result;
}


