#include "../../include/RingBuffer.hpp"
#include "../../include/Constants.hpp"


class TaskDescriptor {

    private:
        // TaskStack *;
        int retval;
        TaskDescriptor * taskParent;
        Constants::STATE taskState;
        char stack[Constants::TD_STACK_SIZE];
    public:

};