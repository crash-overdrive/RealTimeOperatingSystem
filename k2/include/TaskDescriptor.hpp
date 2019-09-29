#ifndef TASK_DESCRIPTOR_HPP
#define TASK_DESCRIPTOR_HPP

#include "Constants.hpp"

class TaskDescriptor {

    public:
        // TaskStack *;
        
        int tid;
        int parentTid;

        Constants::STATE taskState;

        int* sp;
        int cpsr;
        int r0;
        int pc;

        int priority;

        int stack[Constants::TD_STACK_SIZE];

};
#endif