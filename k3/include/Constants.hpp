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
    const int CLOCK_TICK = 5080;
    
    namespace NameServer {
        extern int TID;
        const char REGISTER_AS = 'r';
        const char WHO_IS = 'w';
        const char SUCCESS_REPLY = 's';
        const char FAILURE_REPLY = 'f';
        const int SUCCESS_REPLY_SIZE = 1;
        const int FAILURE_REPLY_SIZE = 1;
        const int SEND_MESSAGE_MAX_SIZE = 16;
        const int REPLY_MESSAGE_MAX_SIZE = sizeof(int);
    }

    namespace ClockServer {
        extern int TID;
    }
    

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