#include "RingBuffer.hpp"
#include "Constants.hpp"


class TaskDescriptor {

    public:
        // TaskStack *;
        int returnValue;
        TaskDescriptor * taskParent;
        Constants::STATE taskState;
        char stack[Constants::TD_STACK_SIZE];
        int stackPointer;
        int cpsr;
        int pc;
    

};