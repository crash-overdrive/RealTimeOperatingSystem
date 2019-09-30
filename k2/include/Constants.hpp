#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace Constants {
    const int NUM_PRIORITIES = 8;
    const int Q_LENGTH = 4;
    const int NUM_TASKS = 128;
    const int EXIT_Q_LENGTH = NUM_TASKS;
    const int RECEIVE_QUEUE_LENGTH = 32;
    const int REPLY_QUEUE_LENGTH = NUM_TASKS;
    const int TD_STACK_SIZE = 32768;
    enum STATE {
        ACTIVE,
        READY,
        SEND_BLOCKED,
        RECEIVE_BLOCKED,
        REPLY_BLOCKED,
        BLOCKED,
        ZOMBIE
    };

    enum RPS_SERVER {
        SIGN_UP,
        PLAY,
        QUIT,
        INVALID
    };
};

#endif