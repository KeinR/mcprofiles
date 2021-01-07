#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include <argh/argh.h>

class Command {
public:
    typedef argh::parser args_t;
    virtual ~Command() = 0;
    virtual void run(const args_t &args) = 0;
};

#endif

