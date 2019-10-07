#include "user/server/ClockServer.hpp"
#include "user/client/ClockNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "kern/TaskDescriptor.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include <string.h>

#define FOREVER for(;;)

class ClockServerEntry {
    public:
        TaskDescriptor* taskDescriptor;
        int ticksToWaitTill;

        ClockServerEntry() {
            taskDescriptor = nullptr;
            ticksToWaitTill = -1;
        }
        
        ClockServerEntry(TaskDescriptor* td, int ticks) {
            taskDescriptor = td;
            ticksToWaitTill = ticks;
        }
};

// INPUTS
// "d" for delay
// "u" for delay-until
// "t" for time
// "x" for tick
// OUTPUTS
// "a" for acknowledge to clockNotifier
void clockServer() {
    bwprintf(COM2, "Clock Server - Created Clock server\n\r");

    RegisterAs("rCLOCK SERVER");

    DataStructures::RingBuffer<ClockServerEntry,Constants::NUM_TASKS> clockServerEntries;
    
    int numberOfTicksElapsed = 0;
    char replyMessage[5] = "R";
    char acknowledge[] = "A";

    int sendTid;
    char sendMessage[5];

    int ticks;
    
    Create(0, clockNotifier); 

    FOREVER {
        int sendSize = Receive(&sendTid, sendMessage, 5);
        if (sendSize == 2 || sendSize == 5) {
            switch (sendMessage[0]) {
            case 'x': // CLOCK NOTIFIER
                ++numberOfTicksElapsed;
                
                bwprintf(COM2, "Clock Server - Received from Notifier: %d ticks\n\r", numberOfTicksElapsed);
                
                Reply(sendTid, acknowledge, 2);
                break;

            case 't': // TIME
                bwprintf(COM2, "Clock Server - Received time Request from: %d at %d ticks\n\r", sendTid, numberOfTicksElapsed);
                
                memcpy(replyMessage+1, &numberOfTicksElapsed, sizeof(numberOfTicksElapsed));
                Reply(sendTid, replyMessage, 5);
                
                break;

            case 'd': // DELAY
                memcpy(&ticks, sendMessage+1, sizeof(ticks));
                
                bwprintf(COM2, "Clock Server - Received delay request from %d for ticks: %d at ticks: %d\n\r", sendTid, ticks, numberOfTicksElapsed);
                
                memcpy(replyMessage+1, &numberOfTicksElapsed, sizeof(numberOfTicksElapsed));
                Reply(sendTid, replyMessage, 5);

                break;

            case 'u': // DELAY UNTIL
                memcpy(&ticks, sendMessage+1, sizeof(ticks));
                
                bwprintf(COM2, "Clock Server - Received delay until request from %d for ticks: %d at ticks: %d\n\r", sendTid, ticks, numberOfTicksElapsed);
                
                memcpy(replyMessage+1, &numberOfTicksElapsed, sizeof(numberOfTicksElapsed));
                Reply(sendTid, replyMessage, 5);

                break;

            default:
                bwprintf(COM2, "Clock Server - Received invalid SEND: %c\n\r", sendMessage[0]);
                
                break;
            }
        } else {
            bwprintf(COM2, "Clock Server - Received invalid send, size: %d\n\r", sendSize);
        }
    }
}