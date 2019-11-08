#ifndef TR_MESSAGE
#define TR_MESSAGE

#include "MessageHeader.hpp"

class TRMessage {
public:
    MessageHeader mh;

    char train;
    char speed;
    bool headlights;

    TRMessage();

    int size();
};

#endif