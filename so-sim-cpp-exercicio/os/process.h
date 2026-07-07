#ifndef PROCESS_H
#define PROCESS_H

#include <cstdint>

namespace OS
{

struct Process
{
    std::string name;
    
    uint16_t pid = 1;

    uint16_t pc = 0;

    bool running = false;
};

}

#endif