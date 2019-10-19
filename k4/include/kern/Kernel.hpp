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

        DataStructures::RingBuffer<TaskDescriptor *, Constants::NUM_TASKS> timerBlockedQueue;

        // These are queues to support the potential for multiple programs being concerned with when uart interrupts have occurred, although in our system only
        // the UART servers are responsible for reading the data
        DataStructures::RingBuffer<TaskDescriptor *, Constants::NUM_TASKS> uart1RXBlockedQueue;
        // DataStructures::RingBuffer<TaskDescriptor, Constants::NUM_TASKS> uart1TXBlocked;
        DataStructures::RingBuffer<TaskDescriptor *, Constants::NUM_TASKS> uart2RXBlockedQueue;
        // DataStructures::RingBuffer<TaskDescriptor, Constants::NUM_TASKS> uart2TXBlocked;
        
        TaskDescriptor tasks[Constants::NUM_TASKS];
        // DataStructures::RingBuffer<TaskDescriptor, Constants::NUM_TASKS> tasks2;

        UART uart1;
        UART uart2;

        TaskDescriptor* activeTask = nullptr;

        volatile int taskNumber = -1;

        TaskDescriptor *haltTD;
        unsigned int timeSpentInIdle = 0;

        void initialize();

        // Draws the kernel's graphical user interface
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
        void handleExit();
        int handleSend(SendRequest *sr);
        int handleReceive(int *tid, int *msg, int msglen);
        int handleReply(int tid, const char *reply, int rplen);
        int handleAwaitEvent(int eventId);

        void handleTimerInterrupt(int timerValue);
        void handleUART1RXInterrupt(int data);
        void handleUART2RXInterrupt(int data);
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