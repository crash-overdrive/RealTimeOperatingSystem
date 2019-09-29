#ifndef MESSAGE_H
#define MESSAGE_H

struct Message {
    int tid;
    const char *msg;
    int msglen;
    char *reply;
    int rplen;
};

#endif
