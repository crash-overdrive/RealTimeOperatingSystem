#ifndef SW_MESSAGE
#define SW_MESSAGE

#include "MessageHeader.hpp"

class SWMessage {
public:
    MessageHeader mh;

    char sw;
    char state;

    SWMessage();

    int size();
};

#endif