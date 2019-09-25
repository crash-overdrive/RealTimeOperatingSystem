#include "../../include/PriorityQueue.hpp"
#include "../../include/Constants.hpp"
#include "../../include/Util.hpp"
#include "TaskDescriptor.cpp"

class Kernel {

    private:
        DataStructures::PriorityQueue<TaskDescriptor, Constants::NUM_PRIORITIES, Constants::Q_LENGTH> ready_queue;
        DataStructures::RingBuffer<TaskDescriptor, Constants::Q_LENGTH> blocked_queue;
        DataStructures::RingBuffer<TaskDescriptor, Constants::EXIT_Q_LENGTH> exit_queue;

        TaskDescriptor tasks[Constants::NUM_TASKS];
        TaskDescriptor* activeTask;

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

            // Kernel exits from here!!
            kernel_exit:

            // Move cpsr of active task into spsr_svc
            asm volatile("msr spsr_svc, %r0" :: "r"(active->cpsr));

            // store kernel state on kernel stack
            asm volatile("stmfd sp_svc! {r0-r12, lr_svc}");
            asm volatile("mrs r12 cpsr");
            asm volatile("stmfd sp_svc! {r12}");

            // Move the next pc into lr_svc
            asm volatile("mov lr_svc, %r0" :: "r"(active->pc));

            // Change to system mode
            // TODO: check if this works
            asm volatile("ldr cpsr 0b11111");

            // TODO:implement this
            // Load registers from user stack
            asm volatile("ldmfd sp_usr! {r4-r11, lr}");

            // Return to supervisor mode
            // TODO: check if this works
            asm volatile("ldr cpsr 0b10011");

            // Set return value by overwriting r0
            asm volatile("mov r0, %r0" :: "r"(active->returnValue));

            // Go back to user mode
            asm volatile("movs pc lr_svc");
            // !!!!!!!!!!!!KERNEL EXITED!!!!!!!!!!!!




            // Should never reach here!! some bug if it reached here!
            Util::assert(false);




            // !!!!!!!!!!!!KERNEL ENTERS!!!!!!!!!!!!
            kernel_entry:
            
            // Change to system mode
            // TODO: check if this works
            asm volatile("ldr cpsr 0b11111");
            
            // Save user state (r4-r11, lr) on user stack  
            // r12 saved by caller
            // sp, pc should not be saved on the stack
            // sp is decremented appropriately and saved into the task descriptor
            // cpsr for user mode is saved in spsr_svc, need to save that into task descriptor as well!!
            asm volatile("stmfd sp_usr! {r4-r11, lr}");
            asm volatile("mov %r0, sp_usr" : "=r"(active->stackPointer));

            // Return to supervisor mode
            // TODO: check if this works
            asm volatile("ldr cpsr 0b10011");

            // Saving cspr of user mode into task descriptor
            asm volatile("mrs %r0, spsr_svc" : "=r"(active->cpsr));

            // Saving pc of user mode into task descriptor
            asm volatile("mov %r0, lr_svc" : "=r"(active->pc));

            // Get Request type
            int requestCode;
            asm volatile("ldr r4, [lr, #-4]");
            asm volatile("mov %r0, r4" : "=r"(requestCode));

            // Get all arguments
            int arg1, arg2, arg3, arg4;
            asm volatile("mov %r0, r0" : "=r"(arg1));
            asm volatile("mov %r0, r1" : "=r"(arg2));
            asm volatile("mov %r0, r2" : "=r"(arg3));
            asm volatile("mov %r0, r3" : "=r"(arg4));

            // Retrieve Kernal State from Kernal Stack
            asm volatile("ldmfd sp_svc! {r12}");
            asm volatile("msr cpsr_svc r12");
            asm volatile("ldmfd sp_svc! {r0-r12, lr_svc}");

            // return requestCode - End of function
            return requestCode;
            
        }

        // TODO: determine if this should return anything
        void handle(int request) {
            // TODO: implement me
        }

};