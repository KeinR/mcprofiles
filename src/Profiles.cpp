#include "Profiles.h"

#include <limits>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <cstdlib>

#include <gzstream.h>

#include "util.h"

#define ASSERT(x) assert(x)

#define SIGNATURE ".mcprofiles"
#define BACKUP "backup"
#define USERS "users"
#define MAX_BACKUPS 20
#define LEVEL_FILE "level.dat"
#define CURRENT_USER "current_user"
#define TMPFILE "mcprofiles423260293264"

#define TAG_End 0x00
#define TAG_Byte 0x01
#define TAG_Short 0x02
#define TAG_Int 0x03
#define TAG_Long 0x04
#define TAG_Float 0x05
#define TAG_Double 0x06
#define TAG_Byte_Array 0x07
#define TAG_String 0x08
#define TAG_List 0x09
#define TAG_Compound 0x0A
#define TAG_Int_Array 0x0B
#define TAG_Long_Array 0x0C

namespace fs = std::filesystem;

Profiles::Profiles(const std::string &path, bool debug, bool verbose): path(path), dbTreeLevel(0), debug(debug), verbose(verbose) {
    fs::path s = this->path / SIGNATURE;
    if (!fs::exists(s)) {
        init();
    } else if (!fs::is_directory(s)) {
        throw std::runtime_error(SIGNATURE " is not a directory");
    }
}

void Profiles::tree(const std::string &str) {
    if (debug) {
        for (int i = 0; i < dbTreeLevel; i++) {
            std::cout << "    ";
        }
        std::cout << str << '\n' << std::flush;
    }
}

void Profiles::status(const std::string &str) {
    if (verbose) {
        std::cout << str << '\n';
    }
}

void Profiles::log(const std::string &str) {
    if (debug) {
        std::cout << str << '\n' << std::flush;
    }
}

std::string newTmpFileName() {
    std::string name = TMPFILE;
    name += std::to_string(std::rand() % 1000000);
    return name;
}

void Profiles::bindump(const std::string &data) {
    if (debug) {
        fs::path tmp0 = fs::temp_directory_path() / newTmpFileName();
        std::ofstream out(tmp0);
        out.write(data.c_str(), data.size());
        out.close();

        std::stringstream command;
        command << "xxd " << tmp0;

        system(command.str().c_str());

        fs::remove(tmp0);
    }
}

void Profiles::levelFileCorrupted() {
    throw std::runtime_error("Level file corrupted");    
}

bool Profiles::confirm() {
    while (true) {
        std::cout << "[y/n]" << std::flush;
        std::string buffer;
        std::cin >> buffer;
        if (buffer == "y") {
            return true;
        } else if (buffer == "n") {
            return false;
        }
        std::cout << '\n';
    }
}

void Profiles::createDir(const fs::path &path) {
    if (!fs::exists(path)) {
        try {
            fs::create_directory(path);
        } catch (std::exception &e) {
            std::cerr << "Error: failed to create directory " << path << '\n';
        }
    }
}

void Profiles::init() {
    status("Initializing...");
    fs::path s = path / SIGNATURE;
    fs::create_directory(s);
    std::ofstream currentUser;
    fs::path cuser = s / CURRENT_USER;
    currentUser.open(cuser);
    currentUser.close();
    if (currentUser.fail()) {
        std::cerr << "Error: failed to touch " << cuser << '\n';
    }
    createDir(s / BACKUP);
    createDir(s / USERS);
    createDir(s / BACKUP / USERS);
}

std::string Profiles::getTimestamp() {
    return std::to_string(getTimestampLong());
}

long Profiles::getTimestampLong() {
    auto time = std::chrono::system_clock::now();
    long epoch = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
    return epoch;
}

std::string Profiles::loadNBT(const std::string &path, bool &failed) {
    gz::igzstream file(path.c_str(), std::istream::binary | std::istream::in);
    std::stringstream buffer;
    buffer << file.rdbuf();
    failed = !file.good();
    return buffer.str();
}

std::string Profiles::loadNBT(const fs::path &path, bool &failed) {
    return loadNBT(path.string(), failed);
}

void Profiles::writeNBT(const std::string &path, const std::string &data, bool &failed) {
    gz::ogzstream file(path.c_str(), std::ostream::binary | std::ostream::out);
    file.write(data.c_str(), data.size());
    failed = !file.good();
}

void Profiles::writeNBT(const fs::path &path, const std::string &data, bool &failed) {
    writeNBT(path.string(), data, failed);
}

unsigned short Profiles::readShort(const str_iterator_t &it) {
    return (*it << 8) | *(it + 1);
}

int Profiles::readInt(const str_iterator_t &it) {
    return (*it << 24) | (*(it + 1) << 16) | (*(it + 2) << 8) | *(it + 3);
}

void Profiles::parseCompound(str_iterator_t &it, const str_iterator_t &end) {
    dbTreeLevel++;

    for (;;) {
        if (it >= end) {
            levelFileCorrupted();
        }
        char type = *it;
        ++it;
        if (type == TAG_End) {
            break;
        }
        unsigned short length = readShort(it);
        it += 2 + length; // Skip name

        tree(std::to_string(int(type)) + ", " + std::string(it - length, it));


        parseUNTag(it, end, type);
    }

    dbTreeLevel--;
}

void Profiles::parseList(str_iterator_t &it, const str_iterator_t &end) {
    int type = *it;
    ++it;
    int length = readInt(it);
    tree("-- Tree len = " + std::to_string(length) + ", type = " + std::to_string(type));
    it += 4;
    if (length == 0) {
        return;
    }
    dbTreeLevel++;
    switch (type) {
        case TAG_End:
            levelFileCorrupted();
            return;
        case TAG_Byte:
            it += 1 * length;
            break;
        case TAG_Short:
            it += 2 * length;
            break;
        case TAG_Int:
            it += 4 * length;
            break;
        case TAG_Long:
            it += 8 * length;
            break;
        case TAG_Float:
            it += 4 * length;
            break;
        case TAG_Double:
            it += 8 * length;
            break;
        case TAG_Byte_Array:
             for (int i = 0; i < length; i++) {
                it += readInt(it) + 4;
            }
            break;
        case TAG_String:
             for (int i = 0; i < length; i++) {
                it += readShort(it) + 2;
            }
            break;
        case TAG_List:
             for (int i = 0; i < length; i++) {
                parseList(it, end);
            }
            break;
        case TAG_Compound:
            for (int i = 0; i < length; i++) {
                parseCompound(it, end);
            }
            break;
        case TAG_Int_Array:
            for (int i = 0; i < length; i++) {
                it += readInt(it) * 4 + 4;
            }
            break;
        case TAG_Long_Array:
            for (int i = 0; i < length; i++) {
                it += readInt(it) * 8 + 4;
            }
            break;
        default:
            levelFileCorrupted();
    }
    if (it >= end) {
        levelFileCorrupted();
    }
    dbTreeLevel--;
}

void Profiles::parseUNTag(str_iterator_t &it, const str_iterator_t &end, int type) {
    switch (type) {
        case TAG_End:
            ASSERT(false);
            break;
        case TAG_Byte:
            it += 1;
            break;
        case TAG_Short:
            it += 2;
            break;
        case TAG_Int:
            it += 4;
            break;
        case TAG_Long:
            it += 8;
            break;
        case TAG_Float:
            it += 4;
            break;
        case TAG_Double:
            it += 8;
            break;
        case TAG_Byte_Array:
            it += readInt(it) + 4;
            break;
        case TAG_String:
            it += readShort(it) + 2;
            break;
        case TAG_List:
            parseList(it, end);
            break;
        case TAG_Compound:
            parseCompound(it, end);
            break;
        case TAG_Int_Array:
            it += readInt(it) * 4 + 4;
            break;
        case TAG_Long_Array:
            it += readInt(it) * 8 + 4;
            break;
        default:
            levelFileCorrupted();
    }
}

Profiles::playerofs_t Profiles::seek(const std::string &data) {

    const std::string tagName = "Player";
    
    playerofs_t player;

    typedef std::string::const_iterator iterator_t;

    bool found = false;

    for (iterator_t it = data.cbegin(); it + 3 < data.cend(); ++it) {
        if (*it == TAG_Compound) {
            iterator_t p = it + 1;
            unsigned short length = readShort(p);
            p += 2;

            log("[seek] Testing length " + std::to_string(length));

            if (length == tagName.size() && p + length < data.end()) {
                std::string name(p, p + length);
                if (tagName == name) {
                    player.first = it; 
                    player.second = p + length;
                    found = true;
                    break; // Tag found
                }
            }
        }
    }

    if (!found) {
        levelFileCorrupted();    
    }

    dbTreeLevel = 0;

    parseCompound(player.second, data.cend());

    return player;
}

std::string Profiles::snapshot() {
    bool err = false;
    std::string data = loadNBT(path / LEVEL_FILE, err);
    playerofs_t pos = seek(data);

    if (err) {
        throw std::runtime_error("Failed to load " LEVEL_FILE);
    }

    return std::string(pos.first, pos.second);
}

void Profiles::backup() {
    status("Backing up level data...");
    fs::path level = path / LEVEL_FILE;
    fs::path backupDir = path / SIGNATURE / BACKUP; 
    fs::create_directories(backupDir);
    fs::path backup = backupDir / getTimestamp();
    fs::copy_file(level, backup);

    fs::path oldest = backup;
    int count = 0;
    long oldestTime = std::numeric_limits<long>::max();
    for (fs::directory_entry e : fs::directory_iterator(backupDir)) {
        fs::path p = e.path();
        if (!fs::is_directory(p)) {
            try {
                long time = std::stol(p.filename().string());
                if (time < oldestTime) {
                    oldestTime = time;
                    oldest = p;
                }
                count++;
            } catch (std::exception &ex) {
                // Invalid file name, ignore...
                log("Warning: Invalid file name " + p.string());
            }
        }
    }

    if (count > MAX_BACKUPS) {
        log("Removing old backup " + oldest.string());
        fs::remove(oldest);
    }
}

void Profiles::save(const std::string &name) {
    std::string data = snapshot();
    fs::path dir = path / SIGNATURE / USERS;
    fs::create_directories(dir);
    fs::path file = dir / name;

    // Ony back up user file if it doesn't exist
    if (fs::exists(file)) {
        status("Backing up old data for " + name + "...");
        fs::path savDir = path / SIGNATURE / BACKUP / USERS;
        fs::create_directories(savDir);
        fs::path savFile = savDir / name;
        fs::copy_file(file, savFile, fs::copy_options::overwrite_existing);
    } else {
        status("Creating new user " + name + "...");
    }

    status("Saving current player data as " + name + "...");

    std::ofstream f(dir / name, std::ofstream::trunc);
    f.write(data.data(), data.size());
}

std::string Profiles::loadFromFile(std::istream &file) {
    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

std::string Profiles::loadFile(const fs::path &p) {
    std::ifstream file;
    file.open(p);
    if (!file.good()) {
        throw std::runtime_error("Failed to open file " + p.string());
    }
    file.exceptions(std::ifstream::badbit);
    return loadFromFile(file);
}

void Profiles::writeFile(const fs::path &p, const std::string &data) {
    std::ofstream out;
    out.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    out.open(p, std::ofstream::trunc);
    out.write(data.data(), data.size());
}

void Profiles::load(const std::string &name) {
    backup(); // Very important

    fs::path currentUserPath = path / SIGNATURE / CURRENT_USER;

    bool isOldUser = false;

    if (fs::exists(currentUserPath)) {
        std::string currentUser = loadFile(currentUserPath);
        if (!currentUser.empty() && !std::any_of(currentUser.begin(), currentUser.end(), ut::notValidUNC)) {
            isOldUser = true;
            save(currentUser);
        }
    }
    if (!isOldUser) {
        std::cerr << "No current user (at " << currentUserPath << "), data will only be saved as backup. Continue?";
        if (!confirm()) {
            std::cout << "Aborting.\n";
            return;
        }
    }

    fs::path p = path / SIGNATURE / USERS / name;
    if (!fs::exists(p)) {
        save(name);
    } else {
        status("Injecting user data for " + name + "...");

        std::string newPlayerData = loadFile(p);

        fs::path dataFile = path / LEVEL_FILE;
        bool err = false;
        std::string levelData = loadNBT(dataFile, err);
        if (err) {
            throw std::runtime_error("Failed to load " LEVEL_FILE);
        }

        log("---------------------- READ DUMP ---------------------");
        bindump(levelData);
        log("-------------------- END READ DUMP -------------------");

        playerofs_t pos = seek(levelData);

        std::string begin(levelData.cbegin(), pos.first);
        std::string end(pos.second, levelData.cend());
        std::string newLevelData;
        newLevelData.reserve(newPlayerData.size() + begin.size() + end.size());
        newLevelData.append(begin.begin(), begin.end());
        newLevelData.append(newPlayerData.begin(), newPlayerData.end());
        newLevelData.append(end.begin(), end.end());

        log("---------------------- WRITE DUMP ---------------------");
        bindump(newLevelData);
        log("-------------------- END WRITE DUMP -------------------");

        bool werr = false;
        writeNBT(dataFile, newLevelData, werr);
        if (werr) {
            throw std::runtime_error("Could not write modified level data file");
        }
       
    }

    status("Setting " + name + " as the current user...");

    writeFile(currentUserPath, name);

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

