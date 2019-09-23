#include "../../include/PriorityQueue.hpp"
#include "../../include/Constants.hpp"
#include "TaskDescriptor.cpp"

class Kernel {

    private:
        DataStructures::PriorityQueue<TaskDescriptor, Constants::NUM_PRIORITIES, Constants::Q_LENGTH> ready_queue;
        DataStructures::RingBuffer<TaskDescriptor, Constants::Q_LENGTH> blocked_queue;
        DataStructures::RingBuffer<TaskDescriptor, Constants::Q_LENGTH> next_queue;

    public:
        // Returns the task descriptor of the next scheduled task
        TaskDescriptor *schedule() {
            return ready_queue.pop();
        }

};