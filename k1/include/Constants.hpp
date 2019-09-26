#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace Constants {
    const int NUM_PRIORITIES = 8;
    const int Q_LENGTH = 4;
    const int EXIT_Q_LENGTH = 128;
    const int TD_STACK_SIZE = 131072 / 4;
    const int NUM_TASKS = 128;
    enum STATE {
        ACTIVE,
        READY,
        BLOCKED,
        ZOMBIE
    };
};

#endif