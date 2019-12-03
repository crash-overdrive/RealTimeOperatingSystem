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
    TRMessage(char tr, char s);

    int size();
};

#endif
