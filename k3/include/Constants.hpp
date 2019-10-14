#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace Constants {
    const int NUM_PRIORITIES = 8;
    const int Q_LENGTH = 10;
    const int NUM_TASKS = 128;
    const int EXIT_Q_LENGTH = NUM_TASKS;
    const int RECEIVE_QUEUE_LENGTH = 32;
    const int REPLY_QUEUE_LENGTH = NUM_TASKS;
    const int TD_STACK_SIZE = 32768;
    const int TASK_NAME_SIZE = 16;
    const int clockTick = 5080;
    enum STATE {
        ACTIVE,
        READY,
        SEND_BLOCKED,
        RECEIVE_BLOCKED,
        REPLY_BLOCKED,
        BLOCKED,
        ZOMBIE,
        TIMER_BLOCKED
    };
    enum SWI {
        CREATE = 2,
        MY_TID,
        MY_PARENT_TID,
        YIELD,
        EXIT,
        SEND,
        RECIEVE,
        REPLY,
        AWAIT_EVENT,
        HALT
    };
    enum EVENTS {
        TIMER_INTERRUPT
    };
};

#endif