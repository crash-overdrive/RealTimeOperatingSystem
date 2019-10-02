#ifndef TASK_DESCRIPTOR_HPP
#define TASK_DESCRIPTOR_HPP

#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "kern/Message.hpp"

class TaskDescriptor {

    public:
        // TaskStack *;
        
        int tid;
        // TaskDescriptor *myTD;
        int parentTid;
        // TaskDescriptor *myParentTD; TODO: Determine is this would be useful

        Constants::STATE taskState;

        int* sp;
        int cpsr;
        int r0;
        int pc;

        int priority;

        int stack[Constants::TD_STACK_SIZE];

        DataStructures::RingBuffer<KernelSendRequest, Constants::RECEIVE_QUEUE_LENGTH> receiveQueue;
        KernelSendRequest kSendRequest;
        KernelRecieveRequest kReceiveRequest;

        char name[33];

};
#endif