#ifndef KERNEL_HPP
#define KERNEL_HPP

#include "PriorityQueue.hpp"
#include "Constants.hpp"
#include "Util.hpp"
#include "TaskDescriptor.hpp"

#define FOREVER for(;;)

extern "C" {
	#include <bwio.h>
}


class Kernel {

    private:
        DataStructures::PriorityQueue<TaskDescriptor, Constants::NUM_PRIORITIES, Constants::Q_LENGTH> ready_queue;
        DataStructures::RingBuffer<TaskDescriptor, Constants::Q_LENGTH> blocked_queue;
        DataStructures::RingBuffer<TaskDescriptor, Constants::EXIT_Q_LENGTH> exit_queue;

        
        TaskDescriptor tasks[Constants::NUM_TASKS];

        volatile TaskDescriptor* activeTask;
        volatile int request;
        int arg1, arg2, arg3, arg4;

        void initialize();

        // Sets the active task to the task descriptor of the next scheduled task.
        void schedule();

        int activate();

        // TODO: determine if this should return anything
        void handle(int request);


    public:

        void run();

};

#endif