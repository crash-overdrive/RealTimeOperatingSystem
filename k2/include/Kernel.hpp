#ifndef KERNEL_HPP
#define KERNEL_HPP

#include "PriorityQueue.hpp"
#include "RingBuffer.hpp"
#include "Constants.hpp"
#include "TaskDescriptor.hpp"
#include "UART.hpp"

#define FOREVER for(;;)

class Kernel {

    private:
        DataStructures::PriorityQueue<TaskDescriptor, Constants::NUM_PRIORITIES, Constants::Q_LENGTH> ready_queue;
        DataStructures::RingBuffer<TaskDescriptor, Constants::Q_LENGTH> blocked_queue;

        DataStructures::RingBuffer<TaskDescriptor, Constants::EXIT_Q_LENGTH> exit_queue;
        DataStructures::RingBuffer<KernelSendRequest, Constants::REPLY_QUEUE_LENGTH> replyQueue;
        
        TaskDescriptor tasks[Constants::NUM_TASKS];

        //TODO: Group all these 3 together?
        UART uart;

        TaskDescriptor* activeTask = nullptr;
        volatile int request;

        void* arg1;
        void* arg2; 
        void* arg3;
        void* arg4;

        volatile int availableTid = 0;

        volatile int taskNumber = -1;

        void initialize();

        // Sets the active task to the task descriptor of the next scheduled task.
        void schedule();

        // Activates the current active task.
        int activate();

        // TODO: determine if this should return anything.
        void handle(int request);

        int handleCreate(int priority, int (*function)());
        int handleMyTid();
        int handleMyParentTid();
        void handleExit();
        int handleSend(SendRequest *sr);
        int handleReceive(int *tid, int *msg, int msglen);
        int handleReply(int tid, const char *reply, int rplen);

        /*
         * Lookup TD by tid
         * 
         * Returns:
         *   TD*    : pointer to task descriptor if tid corresponds to an existing task
         *   nullptr: null otherwise
         */
        TaskDescriptor *lookupTD(int tid);

    public:

        void run();

};

#endif