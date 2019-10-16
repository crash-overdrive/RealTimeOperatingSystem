#ifndef USER_SYSCALL_HPP
#define USER_SYSCALL_HPP

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
int Create(int priority, void (*function)());

/*
 * Returns the task id of the calling task.
 * 
 * Returns:
 *   tid: the positive integer task id of the task that calls it.
 */
int MyTid();

/*
 * Returns the task id of the task that created the calling task. This will be problematic only if the task has exited or been destroyed, in which case the
 * return value is implementation-dependent.
 * 
 * Returns:
 *   tid: the task id of the task that created the calling task.
 */
int MyParentTid( );

/*
 * Causes a task to pause executing. The task is moved to the end of its priority queue, and will resume executing when next scheduled.
 */
void Yield();

/*
 * Causes a task to cease execution permanently. It is removed from all priority queues, send queues, receive queues and event queues. Resources owned by the
 * task, primarily its memory and task descriptor, are not.
 */
void Exit();

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
int Send(int tid, const char *msg, int msglen, char *reply, int rplen);

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
int Receive(int *tid, char *msg, int msglen);

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
int Reply(int tid, const char *reply, int rplen);

/*
 * Registers the task id of the caller under the given name. On return without error it is guaranteed that all WhoIs() calls by any task will return the task id 
 * of the caller until the registration is overwritten. If another task has already registered with the given name, its registration is overwritten. 
 * RegisterAs() is actually a wrapper covering a send to the name server.
 * 
 * Returns:
 *    0: success.
 *   -1: invalid name server task id inside wrapper.
 */
int RegisterAs(const char* name);

/*
 * asks the name server for the task id of the task that is registered under the given name. Whether WhoIs() blocks waiting for a registration or returns 
 * with an error, if no task is registered under the given name, is implementation-dependent. There is guaranteed to be a unique task id associated with 
 * each registered name, but the registered task may change at any time after a call to WhoIs(). WhoIs() is actually a wrapper covering a send to the name server.
 *
 * Returns:
 *   tid: task id of the registered task.
 *    -1: invalid name server task id inside wrapper.
 */
int WhoIs(const char* name);

/*
 * Blocks until the event identified by eventid occurs then returns with volatile data, if any.
 * 
 * Returns:
 *   >-1: volatile data, in the form of a positive integer.
 *    -1: invalid event.
 *    -2: corrupted volatile data.
 */
int AwaitEvent(int eventId);

/*
 * returns the number of ticks since the clock server was created and initialized. With a 10 millisecond tick and a 32-bit unsigned int for the time wraparound is almost 12,000 hours, plenty of time for your demo. Time is actually a wrapper for a send to the clock server. The argument is the tid of the clock server.
 * 
 * Returns:
 *   >-1: time in ticks since the clock server initialized.
 *    -1: tid is not a valid clock server task.
 */
int Time(int tid);

/*
 * returns after the given number of ticks has elapsed. How long after is not guaranteed because the caller may have to wait on higher priority tasks. Delay() is (almost) identical to Yield() if ticks is zero. Delay() is actually a wrapper for a send to the clock server.
 * 
 * Returns:
 *   >-1: success. The current time returned (as in Time())
 *    -1: tid is not a valid clock server task.
 *    -2: negative delay.
 */
int Delay(int tid, int ticks);

/*
 * returns when the time since clock server initialization is greater or equal than the given number of ticks. How long after is not guaranteed because the caller may have to wait on higher priority tasks. Also, DelayUntil(tid, Time(tid) + ticks) may differ from Delay(tid, ticks) by a small amount. DelayUntil is actually a wrapper for a send to the clock server.
 * 
 * Returns:
 *   >-1: success. The current time returned (as in Time())
 *    -1: tid is not a valid clock server task.
 *    -2: negative delay.
 */
int DelayUntil(int tid, int ticks);

/*
 * returns next unreturned character from the given UART. The first argument is the task id of the appropriate server. How communication errors are handled is
 * implementation-dependent. Getc() is actually a wrapper for a send to the appropriate server.
 * 
 * Returns:
 *   >-1: new character from the given UART.
 *    -1: tid is not a valid uart server task.
 */
int Getc(int tid, int uart);

/*
 * queues the given character for transmission by the given UART. On return the only guarantee is that the character has been queued. Whether it has been
 * transmitted or received is not guaranteed. How communication errors are handled is implementation-dependent. Putc() is actually a wrapper for a send to
 * the appropriate server.
 * 
 * Returns:
 *   0: success.
 *  -1: tid is not a valid uart server task.
 */

int Putc(int tid, int uart, char ch);

#endif
