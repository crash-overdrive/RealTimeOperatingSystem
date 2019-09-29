#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace Constants {
    const int NUM_PRIORITIES = 8;
    const int Q_LENGTH = 4;
    const int EXIT_Q_LENGTH = 128;
    const int RECEIVE_QUEUE_LENGTH = 32;
    const int TD_STACK_SIZE = 32768;
    const int NUM_TASKS = 128;
    enum STATE {
        ACTIVE,
        READY,
        SEND_BLOCKED,
        RECEIVE_BLOCKED,
        REPLY_BLOCKED,
        BLOCKED,
        ZOMBIE
    };
};

#endif