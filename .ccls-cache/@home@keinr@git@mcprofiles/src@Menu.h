#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <unordered_map>
#include <string>
#include <memory>

#include "Command.h"

class Menu: public Command {
public:
    typedef std::shared_ptr<Command> value_t;
    typedef std::unordered_map<std::string, value_t> menus_t;
private:
    menus_t menus;
public:
    Menu();
    value_t get(const std::string &name);
    void add(const std::string &name, const value_t &m);
    void run(const args_t &args) override;
};

#endif


