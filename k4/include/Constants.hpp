#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string.h>

namespace Constants {
    const int NUM_PRIORITIES = 10;
    const int Q_LENGTH = 10;
    const int NUM_TASKS = 128;
    const int EXIT_Q_LENGTH = NUM_TASKS;
    const int RECEIVE_QUEUE_LENGTH = 32;
    const int REPLY_QUEUE_LENGTH = NUM_TASKS;
    const int TD_STACK_SIZE = 32768;
    const int MAX_MSGLEN = 512;

    namespace TrainCommandServer {
        const int MAX_SEND_MESSAGE_SIZE = 10;
        const int NUM_TRAINS = 100;
        const int NUM_SENSORS = 200;
        const char ENTER = (char)13;
        const char BACKSPACE = (char)8;
        const char DELIMITER[] = " ";
        const char DIGITS[] = "1234567890";
        const char STRAIGHT_SWITCH_INPUT = 'S';
        const char CURVED_SWITCH_INPUT = 'C';
    }

    namespace SensorData {
        const int NUM_SENSOR_BANKS = 10;
        const int NUM_SENSORS_DISPLAYED = 10;
    }

    namespace MarklinConsole {
        const char STOP_TRAIN = (char)0;
        const char REVERSE_TRAIN = (char)15;
        const char SWITCH_OFF_TURNOUT = (char)32;
        const char STRAIGHT_SWITCH = (char)33;
        const char CURVED_SWITCH = (char)34;
        const char GO = (char)96;
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
        extern volatile int TID;
        const int BUFFER_SIZE = 1024;
        const int MSG_SIZE = 1;
        const int RP_SIZE = 1;
    }

    namespace UART1TXServer {
        extern volatile int TID;
        const int BUFFER_SIZE = 1024;
        const int MSG_SIZE = 1;
        const int RP_SIZE = 1;
    }

    namespace UART2RXServer {
        extern volatile int TID;
        const int BUFFER_SIZE = 1024;
        const int MSG_SIZE = 1;
        const int RP_SIZE = 1;
    }

    namespace UART2TXServer {
        extern volatile int TID;
        const int BUFFER_SIZE = 1024;
        const int MSG_SIZE = 1;
        const int RP_SIZE = 1;
    }

    namespace TerminalServer {
        const int BUFFER_SIZE = 1024;
        const int MSG_SIZE = 128;
        const int RP_SIZE = 128;
        const int CMD_SIZE = 1024;
    }

    // namespace GUIServer {
    //     enum STYLE {
    //         A,
    //         B,
    //         C,
    //     };
    // }

    namespace VT100 {
        const char CLEAR_SCREEN[] = "\033[2J";
        const char SAVE_CURSOR_AND_ATTRS[] = "\0337";
        const char RESTORE_CURSOR_AND_ATTRS[] = "\0338";
        const char MOVE_CURSOR_TO_HOME[] = "\033[H";
        const char MOVE_CURSOR_POS_TO_TIME[] = "\033[2;8f";
        const char MOVE_CURSOR_POS_TO_IDLE[] = "\033[2;23f";
        // const char MOVE_CURSOR_POS_TO_SWITCH[] = "\033[9;4f"; THIS CAN'T BE HARDCODED
        const char MOVE_CURSOR_POS_TO_SENSOR[] = "\033[9;5f";
        const char MOVE_CURSOR_POS_TO_CONSOLE[] = "\033[21;5f";
        const char MOVE_CURSOR_POS_TO_FEEDBACK[] = "\033[22;5f";
        const char MOVE_CURSOR_POS_TO_MSGLOG[] = "\033[25;5f";
        const char SET_UNDERSCORE[] = "\033[4m";
        const char UNSET_UNDERSCORE[] = "\033[24m";
        const char UNSET_BLINK[] = "\033[25m";
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
        TIMER_1_BLOCKED,
        TIMER_2_BLOCKED,
        TIMER_3_BLOCKED,
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
        HALT,
        SWITCH_OFF
    };

    enum EVENTS {
        TIMER_1_INTERRUPT,
        TIMER_2_INTERRUPT,
        TIMER_3_INTERRUPT,
        UART1RX_IRQ,
        UART1TX_IRQ,
        UART2RX_IRQ,
        UART2TX_IRQ,
    };

    enum MSG {
        RDY,
        CHAR,
        TX,
        RX,
        TEXT,
        RESET,
        REQUEST,
        TR,
        RV,
        SW,
        SENSOR,
        SWITCH,
        TRAIN,
        TIME,
        IDLE,
    };
};

#endif