#include "user/server/ClockServer.hpp"
#include "user/client/ClockNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "Constants.hpp"
#include <cstring>

#define FOREVER for(;;)

class ClockServerEntry {
    public:
        int tid;
        int ticksToReleaseAt;

        ClockServerEntry() {
            tid = -1;
            ticksToReleaseAt = -1;
        }
        
        ClockServerEntry(int td, int ticks) {
            tid = td;
            ticksToReleaseAt = ticks;
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

    RegisterAs("CLOCK SERVER");

    // TODO: use a priority queue for this
    DataStructures::RingBuffer<ClockServerEntry,Constants::NUM_TASKS> clockServerEntries;

    // TODO: come up with a better solution for this
    ClockServerEntry tempMemory[Constants::NUM_TASKS];
    int tempCounter = -1;
    
    int numberOfTicksElapsed = 0;
    
    Create(0, clockNotifier);

    FOREVER {
        char replyMessage[Constants::ClockServer::REPLY_MESSAGE_MAX_SIZE];
        replyMessage[0] = Constants::ClockServer::ACKNOWLEDGE;

        int sendTid;
        char sendMessage[Constants::ClockServer::SEND_MESSAGE_MAX_SIZE];

        int sendMessageSize = Receive(&sendTid, sendMessage, Constants::ClockServer::SEND_MESSAGE_MAX_SIZE);

        if (sendMessageSize == 1 || sendMessageSize == 5) {
            switch (sendMessage[0]) {
            case Constants::ClockServer::TICK:
                {
                    ++numberOfTicksElapsed;

                    // bwprintf(COM2, "Clock Server - Received from Notifier: %d ticks\n\r", numberOfTicksElapsed);
                    Reply(sendTid, replyMessage, 1);

                    // iterate through clockServerEntries and reply to the elements which are ready
                    int length = clockServerEntries.size();
                    // bwprintf(COM2, "Clock Server - Length of clockServerEntries is %d\n\r", length);
                    for (int i=0; i < length; ++i) {
                        ClockServerEntry clockServerEntry = clockServerEntries.pop();
                        // bwprintf(COM2, "Clock Server - Popped off entry %d %d\n\r", clockServerEntry->tid, clockServerEntry->ticksToReleaseAt);
                        if (clockServerEntry.ticksToReleaseAt <= numberOfTicksElapsed) {
                            // bwprintf(COM2, "Clock Server - Releasing %d from sleep at ticks: %d\n\r", clockServerEntry->tid, numberOfTicksElapsed);
                            memcpy(replyMessage+1, &numberOfTicksElapsed, sizeof(numberOfTicksElapsed));
                            Reply(clockServerEntry.tid, replyMessage, 5);
                        } else {
                            clockServerEntries.push(clockServerEntry);
                        }
                    }
                    break;
                }
                

            case Constants::ClockServer::TIME:
                {
                    // bwprintf(COM2, "Clock Server - Received time Request from: %d at %d ticks\n\r", sendTid, numberOfTicksElapsed);

                    memcpy(replyMessage+1, &numberOfTicksElapsed, sizeof(numberOfTicksElapsed));
                    Reply(sendTid, replyMessage, 5);

                    break;
                }
                

            case Constants::ClockServer::DELAY:
                {
                    int ticks;
                    memcpy(&ticks, sendMessage+1, sizeof(ticks));

                    // bwprintf(COM2, "Clock Server - Received delay request from %d for ticks: %d at ticks: %d, pushed to cses\n\r", sendTid, ticks, numberOfTicksElapsed);

                    ++tempCounter;
                    tempMemory[tempCounter] = ClockServerEntry(sendTid, ticks + numberOfTicksElapsed);
                    clockServerEntries.push(tempMemory[tempCounter]);

                    break;
                }
                

            case Constants::ClockServer::DELAY_UNTIL:
                {
                    int ticks;
                    memcpy(&ticks, sendMessage+1, sizeof(ticks));
                    
                    // bwprintf(COM2, "Clock Server - Received delay until request from %d for ticks: %d at ticks: %d, pushed to cses\n\r", sendTid, ticks, numberOfTicksElapsed);
                    
                    ++tempCounter;
                    tempMemory[tempCounter] = ClockServerEntry(sendTid, ticks);
                    clockServerEntries.push(tempMemory[tempCounter]);

                    break;
                }
                

            default:
                {
                    bwprintf(COM2, "Clock Server - Received invalid SEND: %d, SendMessageSize: %d\n\r", sendMessage[0], sendMessageSize);
                    
                    break;
                }
                    
            }
        } else {
            bwprintf(COM2, "Clock Server - Received invalid send, size: %d\n\r", sendMessageSize);
        }
    }
}