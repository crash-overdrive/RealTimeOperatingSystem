#include "user/server/ClockServer.hpp"
#include "user/client/ClockNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "kern/TaskDescriptor.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"

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
    char acknowledge[] = "a";
    char replyMessage[2] = "A";
    int numberOfTicks = 0;

    int sendTid;
    char sendMessage[2];
    
    Create(0, clockNotifier); 

    

    FOREVER {
        int sendSize = Receive(&sendTid, sendMessage, 2);
        if (sendSize == 2) {
            switch (sendMessage[0])
            {
            case 'x':
                ++numberOfTicks;
                bwprintf(COM2, "Clock Server - Received from Notifier, Tick Tock: %d\n\r", numberOfTicks);
                Reply(sendTid, "a", 2);
                break;

            case 't':
                replyMessage[0] = (char)numberOfTicks;
                bwprintf(COM2, "Clock Server - Received time Request from: %d %d ticks\n\r", sendTid, numberOfTicks);
                Reply(sendTid, replyMessage, 2);
                break;

            case 'u':
                bwprintf(COM2, "Clock Server - Received delay until request\n\r");
                break;

            case 'd':
                bwprintf(COM2, "Clock Server - Received delay request\n\r");
                break;
            
            default:
                bwprintf(COM2, "Clock Server - Received invalid SEND: %c\n\r", sendMessage[0]);
                break;
            }
        }
        
    }
}