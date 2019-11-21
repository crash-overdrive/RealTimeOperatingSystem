#ifndef TP_MESSAGE
#define TP_MESSAGE

#include "MessageHeader.hpp"

class TPMessage {
public:
    MessageHeader mh;

    char train;
    char src[6];

    TPMessage();

    int size();
};

#endif
