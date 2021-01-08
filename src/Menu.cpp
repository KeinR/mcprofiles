#include "Menu.h"

#include <iostream>

Menu::Menu() {
}

Menu::value_t Menu::get(const std::string &name) {
    menus_t::iterator it = menus.find(name);
    if (it != menus.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

void Menu::add(const std::string &name, const value_t &m) {
    menus.emplace(name, m);
}

void Menu::run(const args_t &args) {
    
}

