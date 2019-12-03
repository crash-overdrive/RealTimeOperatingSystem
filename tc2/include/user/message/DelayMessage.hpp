#ifndef DELAY_MESSAGE
#define DELAY_MESSAGE

#include "MessageHeader.hpp"

class DelayMessage {
public:
    MessageHeader mh;
    unsigned short delay;

    DelayMessage();

    int size();
};

#endif
