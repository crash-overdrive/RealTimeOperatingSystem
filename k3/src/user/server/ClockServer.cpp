#include "user/server/ClockServer.hpp"
#include "user/client/ClockNotifier.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "kern/TaskDescriptor.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"

#define FOREVER for(;;)

// INPUTS
// "d" for delay 
// "u" for delay-until
// "t" for time
// "x" for tick
// OUTPUTS
// "a" for acknowledge to clockNotifier

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

void clockServer() {
    bwprintf(COM2, "Created Clock server\n\r");
    DataStructures::RingBuffer<ClockServerEntry,Constants::NUM_TASKS> clockServerEntries;
    char acknowledge[] = "a";

    int sendTid;
    char sendMessage[2];
    
    Create(0, clockNotifier); 

    

    FOREVER {
        int sendSize = Receive(&sendTid, sendMessage, 2);
        bwprintf(COM2, "Tick Tock: %d %d %s\n\r", sendSize, sendTid, sendMessage);

    }
}