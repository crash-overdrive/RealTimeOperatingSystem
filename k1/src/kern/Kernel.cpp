#include "../../include/PriorityQueue.hpp"
#include "../../include/Constants.hpp"
#include "TaskDescriptor.cpp"

class Kernel {

    private:
        DataStructures::PriorityQueue<TaskDescriptor, Constants::NUM_PRIORITIES, Constants::Q_LENGTH> ready_queue;
        DataStructures::RingBuffer<TaskDescriptor, Constants::Q_LENGTH> blocked_queue;
        DataStructures::RingBuffer<TaskDescriptor, Constants::EXIT_Q_LENGTH> exit_queue;

        TaskDescriptor tasks[Constants::NUM_TASKS];

    public:
        void initialize() {
            // TODO: implement me
            // setup comm
        }

        // Returns the task descriptor of the next scheduled task
        TaskDescriptor *schedule() {
            return ready_queue.pop();
        }

        int activate(TaskDescriptor *active) {
            // TODO: implement me
        }

        // TODO: determine if this should return anything
        void handle(int request) {
            // TODO: implement me
        }

};