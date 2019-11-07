#ifndef KERNEL_HPP
#define KERNEL_HPP

#include "data-structures/PriorityQueue.hpp"
#include "data-structures/RingBuffer.hpp"
#include "Constants.hpp"
#include "kern/TaskDescriptor.hpp"
#include "io/UART.hpp"

#define FOREVER for(;;)

#define TRAP for(;;) {}

class Kernel {

    private:
        DataStructures::PriorityQueue<TaskDescriptor *, Constants::NUM_PRIORITIES, Constants::Q_LENGTH> ready_queue;
        DataStructures::RingBuffer<TaskDescriptor *, Constants::Q_LENGTH> blocked_queue;
        DataStructures::RingBuffer<TaskDescriptor *, Constants::EXIT_Q_LENGTH> exit_queue;
        DataStructures::RingBuffer<KernelSendRequest *, Constants::REPLY_QUEUE_LENGTH> replyQueue;

        DataStructures::RingBuffer<TaskDescriptor *, Constants::NUM_TASKS> timer1BlockedQueue;
        DataStructures::RingBuffer<TaskDescriptor *, Constants::NUM_TASKS> timer2BlockedQueue;
        DataStructures::RingBuffer<TaskDescriptor *, Constants::NUM_TASKS> timer3BlockedQueue;

        // These are queues to support the potential for multiple programs being concerned with when uart interrupts have occurred, although in our system only
        // the UART servers are responsible for reading the data
        DataStructures::RingBuffer<TaskDescriptor *, Constants::NUM_TASKS> uart1RXBlockedQueue;
        DataStructures::RingBuffer<TaskDescriptor *, Constants::NUM_TASKS> uart1TXBlockedQueue;
        DataStructures::RingBuffer<TaskDescriptor *, Constants::NUM_TASKS> uart2RXBlockedQueue;
        DataStructures::RingBuffer<TaskDescriptor *, Constants::NUM_TASKS> uart2TXBlockedQueue;

        TaskDescriptor tasks[Constants::NUM_TASKS];

        UART uart1;
        UART uart2;

        TaskDescriptor* activeTask = nullptr;

        volatile int taskNumber = -1;

        TaskDescriptor *haltTD;
        unsigned int timeSpentInIdleInThisTick = 0;
        unsigned int timeSpentInIdleInLastTick = 0;

        void initialize();
        void drawGUI();

        // Sets the active task to the task descriptor of the next scheduled task.
        void schedule();

        // Activates the current active task.
        int* activate();

        // Handles SWIs and IRQs
        void handle(int* stackPointer);

        int handleCreate(int priority, void (*function)());
        int handleMyTid();
        int handleMyParentTid();
        int handleSend(SendRequest *sr);
        int handleReceive(int *tid, char *msg, int msglen);
        int handleReply(int tid, const char *reply, int rplen);
        int handleAwaitEvent(int eventId);
        int handleHalt();
        void handleExit();
        void handleSwitchOff();

        // void handleInterrupt(int data, DataStructures::RingBuffer<TaskDescriptor, Constants::NUM_TASKS> &blockQueue);
        void handleInterrupt(DataStructures::RingBuffer<TaskDescriptor *, Constants::NUM_TASKS> &blockQueue);

        /*
         * Lookup TD by tid
         *
         * Returns:
         *   TD*    : pointer to task descriptor if tid corresponds to an existing task
         *   nullptr: null otherwise
         */
        TaskDescriptor* lookupTD(int tid);

    public:
        Kernel();

        void run();

};

#endif
