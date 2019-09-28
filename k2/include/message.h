#ifndef MESSAGE_H
#define MESSAGE_H

struct message {
    int tid;
    const char *msg;
    int msglen;
    char *reply;
    int rplen;
};

#endif
