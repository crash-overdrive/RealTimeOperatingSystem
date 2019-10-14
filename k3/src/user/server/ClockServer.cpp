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
        int tid;
        int ticksToWaitTill;

        ClockServerEntry() {
            tid = -1;
            ticksToWaitTill = -1;
        }
        
        ClockServerEntry(int td, int ticks) {
            tid = td;
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
    // bwprintf(COM2, "Clock Server - Created Clock server\n\r");

    int x = RegisterAs("CLOCK SERVER");

    // TODO: use a priority queue for this
    DataStructures::RingBuffer<ClockServerEntry,Constants::NUM_TASKS> clockServerEntries;

    // TODO: fix this..
    ClockServerEntry tempMemory[Constants::NUM_TASKS];
    int tempCounter = -1;
    
    int numberOfTicksElapsed = 0;
    char replyMessage[5] = "R";
    char acknowledge[] = "A";

    int sendTid;
    char sendMessage[5];

    int ticks;

    ClockServerEntry* clockServerEntry;
    
    Create(0, clockNotifier);

    FOREVER {
        int sendSize = Receive(&sendTid, sendMessage, 5);
        if (sendSize == 2 || sendSize == 5) {
            switch (sendMessage[0]) {
            case 'x': // CLOCK NOTIFIER
                {
                    ++numberOfTicksElapsed;

                    // bwprintf(COM2, "Clock Server - Received from Notifier: %d ticks\n\r", numberOfTicksElapsed);
                    Reply(sendTid, acknowledge, 2);

                    // iterate through clockServerEntries and reply to the elements which are ready
                    int length = clockServerEntries.size();
                    // bwprintf(COM2, "Clock Server - Length of clockServerEntries is %d\n\r", length);
                    for (int i=0; i < length; ++i) {
                        clockServerEntry = clockServerEntries.pop();
                        // bwprintf(COM2, "Clock Server - Popped off entry %d %d\n\r", clockServerEntry->tid, clockServerEntry->ticksToWaitTill);
                        clockServerEntry->ticksToWaitTill -= 1;
                        // bwprintf(COM2, "Clock Server - Reducing sleep time for %d to %d\n\r", clockServerEntry->tid, clockServerEntry->ticksToWaitTill);
                        if (clockServerEntry->ticksToWaitTill <= 0) {
                            // bwprintf(COM2, "Clock Server - Releasing %d from sleep at ticks: %d\n\r", clockServerEntry->tid, numberOfTicksElapsed);
                            memcpy(replyMessage+1, &numberOfTicksElapsed, sizeof(numberOfTicksElapsed));
                            Reply(clockServerEntry->tid, replyMessage, 5);
                        } else {
                            clockServerEntries.push(clockServerEntry);
                        }
                    }
                    break;
                }
                

            case 't': // TIME
                {
                    // bwprintf(COM2, "Clock Server - Received time Request from: %d at %d ticks\n\r", sendTid, numberOfTicksElapsed);

                    memcpy(replyMessage+1, &numberOfTicksElapsed, sizeof(numberOfTicksElapsed));
                    Reply(sendTid, replyMessage, 5);

                    break;
                }
                

            case 'd': // DELAY 
                {
                    memcpy(&ticks, sendMessage+1, sizeof(ticks));

                    // bwprintf(COM2, "Clock Server - Received delay request from %d for ticks: %d at ticks: %d, pushed to cses\n\r", sendTid, ticks, numberOfTicksElapsed);

                    ++tempCounter;
                    tempMemory[tempCounter] = ClockServerEntry(sendTid, ticks);
                    clockServerEntries.push(&tempMemory[tempCounter]);

                    break;
                }
                

            case 'u': // DELAY UNTIL
                {
                    memcpy(&ticks, sendMessage+1, sizeof(ticks));
                    
                    // bwprintf(COM2, "Clock Server - Received delay until request from %d for ticks: %d at ticks: %d, pushed to cses\n\r", sendTid, ticks, numberOfTicksElapsed);
                    
                    ++tempCounter;
                    tempMemory[tempCounter] = ClockServerEntry(sendTid, ticks - numberOfTicksElapsed);
                    clockServerEntries.push(&tempMemory[tempCounter]);

                    break;
                }
                

            default:
                {
                    bwprintf(COM2, "Clock Server - Received invalid SEND: %c\n\r", sendMessage[0]);
                    
                    break;
                }
                    
            }
        } else {
            bwprintf(COM2, "Clock Server - Received invalid send, size: %d\n\r", sendSize);
        }
    }
}