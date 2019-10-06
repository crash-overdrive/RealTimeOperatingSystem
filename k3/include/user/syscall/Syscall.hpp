#ifndef SYSCALL_HPP
#define SYSCALL_HPP

#include "kern/Message.hpp"
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

/*
 * Sends a message to another task and receives a reply. The message, in a buffer in the sending task’s memory, is copied to the memory of the task to which
 * it is sent by the kernel. Send() supplies a buffer into which the reply is to be copied, and the size of the reply buffer, so that the kernel can detect
 * overflow. When Send() returns without error it is guaranteed that the message has been received, and that a reply has been sent, not necessarily by the
 * same task. The kernel will not overflow the reply buffer. If the size of the reply set exceeds rplen, the reply is truncated and the buffer contains the
 * first rplen characters of the reply. The caller is expected to check the return value and to act accordingly. There is no guarantee that Send() will return.
 * If, for example, the task to which the message is directed never calls Receive(), Send() never returns and the sending task remains blocked forever. Send()
 * has a passing resemblance, and no more, to a remote procedure call.
 * 
 * Returns:
 *   size: the size of the message returned by the replying task. The actual reply is less than or equal to the size of the reply buffer provided for it.
 *         Longer replies are truncated.
 *   -1  : tid is not the task id of an existing task.
 *   -2  : send-receive-reply transaction could not be completed.
 */
int sysSend(SendRequest *sendRequest);

/*
 * Blocks until a message is sent to the caller, then returns with the message in its message buffer and tid set to the task id of the task that sent the
 * message. Messages sent before Receive() is called are retained in a send queue, from which they are received in first-come, first-served order. The
 * argument msg must point to a buffer at least as large as msglen. The kernel will not overflow the message buffer. If the size of the message set exceeds
 * msglen, the message is truncated and the buffer contains the first msglen characters of the message sent. The caller is expected to check the return value
 * and to act accordingly.
 * 
 * Returns:
 *   size: the size of the message sent by the sender (stored in tid). The actual message is less than or equal to the size of the message buffer supplied.
 *         Longer messages are truncated.
 */
int sysReceive(int *tid, char *msg, int msglen);

/*
 * Sends a reply to a task that previously sent a message. When it returns without error, the reply has been copied into the sender’s memory. The calling task
 * and the sender return at the same logical time, so whichever is of higher priority runs first. If they are of the same priority, the sender runs first.
 * 
 * Returns:
 *    0: success.
 *   -1: reply message was truncated.
 *   -2: tid is not the task id of an existing task.
 *   -3: tid is not the task id of a reply-blocked task.
 */
int sysReply(int tid, const char *reply, int rplen);

/*
 * blocks until the event identified by eventid occurs then returns with volatile data, if any.
 * 
 * Return Value
 * >-1	volatile data, in the form of a positive integer.
 * -1	invalid event.
 * -2	corrupted volatile data. 
 * 
*/
int sysAwaitEvent(int eventId);
#endif
