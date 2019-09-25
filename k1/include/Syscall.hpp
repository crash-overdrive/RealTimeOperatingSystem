#ifndef SYSCALL_HPP
#define SYSCALL_HPP

/*
 * Allocates and initializes a task descriptor, using the given priority, and the given function pointer as a pointer to the entry point of executable code,
 * essentially a function with no arguments and no return value. When Create returns, the task descriptor has all the state needed to run the task, the task’s
 * stack has been suitably initialized, and the task has been entered into its ready queue so that it will run the next time it is scheduled.
 * 
 * Returns:
 *   tid: the positive integer task id of the newly created task. The task id must be unique, in the sense that no other active task has the same task id. 
 *   -1 : invalid priority
 *   -2 : kernel is out of task descriptors
 */
int sysCreate(int priority, void (*function)());

/*
 * Returns the task id of the calling task.
 * 
 * Returns:
 *   tid: the positive integer task id of the task that calls it.
 */
int sysMyTid();

/*
 * Returns the task id of the task that created the calling task. This will be problematic only if the task has exited or been destroyed, in which case the
 * return value is implementation-dependent.
 * 
 * Returns:
 *   tid: the task id of the task that created the calling task.
 */
int sysMyParentTid();

/*
 * Causes a task to pause executing. The task is moved to the end of its priority queue, and will resume executing when next scheduled.
 */
void sysYield();

/*
 * Causes a task to cease execution permanently. It is removed from all priority queues, send queues, receive queues and event queues. Resources owned by the
 * task, primarily its memory and task descriptor, are not.
 */
void sysExit();

#endif