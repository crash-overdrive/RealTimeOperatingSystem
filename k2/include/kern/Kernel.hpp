#ifndef KERNEL_HPP
#define KERNEL_HPP

#include "data-structures/PriorityQueue.hpp"
#include "data-structures/RingBuffer.hpp"
#include "Constants.hpp"
#include "kern/TaskDescriptor.hpp"
#include "io/UART.hpp"

#define FOREVER for(;;)

class Kernel {

    private:
        DataStructures::PriorityQueue<TaskDescriptor, Constants::NUM_PRIORITIES, Constants::Q_LENGTH> ready_queue;
        DataStructures::RingBuffer<TaskDescriptor, Constants::Q_LENGTH> blocked_queue;

        DataStructures::RingBuffer<TaskDescriptor, Constants::EXIT_Q_LENGTH> exit_queue;
        DataStructures::RingBuffer<KernelSendRequest, Constants::REPLY_QUEUE_LENGTH> replyQueue;
        
        TaskDescriptor tasks[Constants::NUM_TASKS];

        UART uart;

        TaskDescriptor* activeTask = nullptr;

        volatile int availableTid = -1;

        volatile int taskNumber = -1;

        void initialize();

        // Sets the active task to the task descriptor of the next scheduled task.
        void schedule();

        // Activates the current active task.
        int* activate();

        // TODO: determine if this should return anything.
        void handle(int* stackPointer);

        int handleCreate(int priority, void (*function)());
        int handleMyTid();
        int handleMyParentTid();
        void handleExit();
        int handleSend(SendRequest *sr);
        int handleReceive(int *tid, int *msg, int msglen);
        int handleReply(int tid, const char *reply, int rplen);
        int handleRegisterAs(const char* name);
        int handleWhoIs(const char* name);

        /*
         * Lookup TD by tid
         * 
         * Returns:
         *   TD*    : pointer to task descriptor if tid corresponds to an existing task
         *   nullptr: null otherwise
         */
        TaskDescriptor* lookupTD(int tid);

    public:

        void run();

};

#endif