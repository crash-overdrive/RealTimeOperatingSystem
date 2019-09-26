#include "RingBuffer.hpp"
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
        void* pc;

        int priority;

        char stack[Constants::TD_STACK_SIZE];

};