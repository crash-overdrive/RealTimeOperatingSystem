#include "RingBuffer.hpp"
#include "Constants.hpp"


class TaskDescriptor {

    public:
        // TaskStack *;
        
        int selfTid;
        int parentTid;

        Constants::STATE taskState;

        int* sp;
        int cpsr;
        int pc;
        int r0;

        int priority;

        char stack[Constants::TD_STACK_SIZE];
    

};