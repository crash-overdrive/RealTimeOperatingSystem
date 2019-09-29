#ifndef MESSAGE_HPP
#define MESSAGE_HPP

class TaskDescriptor;

class SendRequest {
public:
    int tid;
    const char *msg;
    int msglen;
    char *reply;
    int rplen;
};

class KernelSendRequest {
public:
    TaskDescriptor *receiverTD;
    TaskDescriptor *senderTD;
    SendRequest *sendRequest;
};

#endif
