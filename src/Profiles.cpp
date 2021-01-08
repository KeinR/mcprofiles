#include "Profiles.h"

#include <limits>
#include <stdexcept>

#define SIGNATURE ".mcprofiles"
#define BACKUP "backup"
#define USERS "users"
#define MAX_BACKUPS 20
#define LEVEL_FILE "level.dat"
#define CURRENT_USER "current_user"

#define TAG_End 0x00
#define TAG_Compound 0x0A
#define TAG_Byte 0x01

namespace fs = std::filesystem;

Profiles::Profiles(const std::string &path): path(path) {
    fs::path s = this->path / SIGNATURE;
    if (!fs::exists(s)) {
        init();
    } else if (fs::is_directory(s)) {
        throw std::runtime_error(SIGNATURE " is not a directory");
    }
}

void Profiles::init() {
    fs::path s = path / SIGNATURE;
    fs::create_directory(s);
    std::ofstream currentUser;
    currentUser.exceptions(std::ofstream::badbit | std::ofstream::failbit);
    currentUser.open(s / CURRENT_USER);
    currentUser.close();
    fs::create_directory(s / BACKUP);
    fs::create_directory(s / USERS);
    fs::create_directory(s / BACKUP / USERS);
}

std::string Profiles::getTimestamp() {
    auto time = std::chrono::system_clock::now();
    long epoch = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
    return std::to_string(epoch);
}

Profiles::playerofs_t Profiles::seek(std::ifstream &file) {

    const std::string tagName = "Player";
    
    playerofs_t player;

    file.seekg(0, file.beg);

    while (file.good()) {
        if (file.get() == TAG_Compound) {
            unsigned short length;
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
            unsigned short length;
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

    file.seekg(0, file.beg);

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

void Profiles::save(const std::string &name) {
    std::string data = snapshot();
    fs::path dir = path / SIGNATURE / USERS;
    fs::create_directories(dir);
    fs::path file = dir / name;
    if (fs::is_directory(file)) {
        throw std::invalid_argument("User file is a directory (?)");
    }

    fs::path savDir = path / SIGNATURE / BACKUP / USERS;
    fs::create_directories(savDir);
    fs::path savFile = savDir / name;
    fs::copy_file(file, savFile);
    
    std::ofstream f(dir / name, std::ofstream::trunc);
    f.write(data.data(), data.size());
}

std::vector<char> Profiles::loadFile(std::ifstream &file) {
    file.seekg(0, file.end);
    std::size_t len = file.tellg();
    file.seekg(0, file.beg);
    std::vector<char> buffer(len);
    file.read(buffer.data(), buffer.size());
    file.seekg(0, file.beg);
    return buffer;
}

void Profiles::load(const std::string &name) {
    fs::path p = path / SIGNATURE / USERS / name;
    if (!fs::exists(p)) {
        save(name);
    } else {
        backup(); // Very important

        std::ifstream in;
        in.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        
        fs::path currentUserPath = path / SIGNATURE / CURRENT_USER;
        in.open(currentUserPath);
        std::vector<char> currentUserBuf = loadFile(in);
        std::string currentUser(currentUserBuf.begin(), currentUserBuf.end());
        in.close();
        in.clear();
        if (!currentUser.empty()) {
            save(currentUser);
        }

        in.open(p, std::ifstream::binary);
        std::vector<char> newPlayerData = loadFile(in);
        in.close();
        in.clear();

        fs::path dataFile = path / LEVEL_FILE;
        in.open(dataFile, std::ifstream::binary);
        playerofs_t pos = seek(in);
        std::vector<char> levelData = loadFile(in);
        in.close();

        std::vector<char> begin(levelData.begin(), levelData.begin() + pos.first);
        std::vector<char> end(levelData.begin() + pos.second, levelData.end());
        std::vector<char> newLevelData;
        newLevelData.reserve(newPlayerData.size() + begin.size() + end.size());
        newLevelData.insert(newLevelData.end(), begin.begin(), begin.end());
        newLevelData.insert(newLevelData.end(), newPlayerData.begin(), newPlayerData.end());
        newLevelData.insert(newLevelData.end(), end.begin(), end.end());

        std::ofstream out;
        out.exceptions(std::ofstream::badbit | std::ofstream::failbit);

        out.open(dataFile, std::ofstream::binary | std::ofstream::trunc);
        out.write(newLevelData.data(), newLevelData.size());
        out.close();
        out.clear();

        out.open(currentUserPath, std::ofstream::trunc);
        out.write(name.data(), name.size());
        out.close();
        out.clear();
        
    }


}

void Profiles::remove(const std::string &name) {
    fs::remove(path / SIGNATURE / USERS / name);
}

std::vector<std::string> Profiles::getList() {
    std::vector<std::string> results;
    for (auto e : fs::directory_iterator(path / SIGNATURE / USERS)) {
        results.push_back(e.path().filename().string());
    }
    return results;
}

