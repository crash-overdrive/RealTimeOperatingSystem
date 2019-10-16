#ifndef TASK_DESCRIPTOR_HPP
#define TASK_DESCRIPTOR_HPP

#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "kern/Message.hpp"

class TaskDescriptor {

    public:
        // TODO: Determine is this would be useful
        // TaskDescriptor *myParentTD; 
        int tid;
        int parentTid;
        Constants::STATE taskState;
        int priority;

        int* sp;
        int returnValue;
        int stack[Constants::TD_STACK_SIZE];

        DataStructures::RingBuffer<KernelSendRequest*, Constants::RECEIVE_QUEUE_LENGTH> receiveQueue;
        KernelSendRequest kSendRequest;
        KernelRecieveRequest kReceiveRequest;
};
#endif