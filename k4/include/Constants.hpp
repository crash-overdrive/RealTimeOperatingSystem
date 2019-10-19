#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string.h>

namespace Constants {
    const int NUM_PRIORITIES = 8;
    const int Q_LENGTH = 10;
    const int NUM_TASKS = 128;
    const int EXIT_Q_LENGTH = NUM_TASKS;
    const int RECEIVE_QUEUE_LENGTH = 32;
    const int REPLY_QUEUE_LENGTH = NUM_TASKS;
    const int TD_STACK_SIZE = 32768;

    namespace ManualTrainControl {
        const int MAX_COMMAND_SIZE = 10;
        const int NUM_TRAINS = 100;
        const int NUM_SENSORS = 200;
        const char ENTER = (char)13;
        const char BACKSPACE = (char)8;
        const char DELIMITER[] = " ";
        const char DIGITS[] = "1234567890";
        const char STRAIGHT_SWITCH_INPUT = 'S';
        const char CURVED_SWITCH_INPUT = 'C';
        
    }

    namespace MarklinConsole {
        const char STOP_TRAIN = (char)0;
        const char REVERSE_TRAIN = (char)15;
        const char SWITCH_OFF_TURNOUT = (char)32;
        const char STRAIGHT_SWITCH = (char)33;
        const char CURVED_SWITCH = (char)34;
        const char START = (char)96;
        const char STOP = (char)97;
        const char REQUEST_5_SENSOR_DATA = (char)133;
        const char SET_RESET_ON_FOR_SENSORS = (char)192;
    }
    
    namespace Server {
        const char ACK = 'A';
        const char ERR = 'E';
    }
    
    namespace NameServer {
        extern int TID;
        const char REGISTER_AS = 'r';
        const char WHO_IS = 'w';
        const char SUCCESS_REPLY[] = "s";
        const char FAILURE_REPLY[] = "f";
        const int SUCCESS_REPLY_SIZE = strlen(SUCCESS_REPLY) + 1;
        const int FAILURE_REPLY_SIZE = strlen(FAILURE_REPLY) + 1;
        const int SEND_MESSAGE_MAX_SIZE = 16;
        const int REPLY_MESSAGE_MAX_SIZE = sizeof(int);
    }

    namespace ClockServer {
        extern int TID;
        const char TIME = 't';
        const char DELAY = 'd';
        const char DELAY_UNTIL = 'u';
        const char TICK = 'x';
        const char ACKNOWLEDGE = 'a';
        const int SEND_MESSAGE_MAX_SIZE = sizeof(int) + 1;
        const int REPLY_MESSAGE_MAX_SIZE = sizeof(int) + 1;
        const int CLOCK_TICK = 5080;
    }

    namespace UART1RXServer {
        extern int TID;
        const int BUFFER_SIZE = 1024;
        const int MSG_SIZE = 1;
        const int RP_SIZE = 1;
    }

    namespace UART1TXServer {
        extern int TID;
        const int BUFFER_SIZE = 1024;
        const int MSG_SIZE = 1;
        const int RP_SIZE = 1;
    }

    namespace UART2RXServer {
        extern int TID;
        const int BUFFER_SIZE = 1024;
        const int MSG_SIZE = 1;
        const int RP_SIZE = 1;
    }

    namespace UART2TXServer {
        extern int TID;
        const int BUFFER_SIZE = 1024;
        const int MSG_SIZE = 1;
        const int RP_SIZE = 1;
    }


    namespace RockPaperScissorServer {
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
        TIMER_BLOCKED,
        UART1RX_BLOCKED,
        UART1TX_BLOCKED,
        UART2RX_BLOCKED,
        UART2TX_BLOCKED,
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
        TIMER_INTERRUPT,
        UART1RX_IRQ,
        UART1TX_IRQ,
        UART2RX_IRQ,
        UART2TX_IRQ,
    };
};

#endif