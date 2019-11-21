#ifndef RT_MESSAGE
#define RT_MESSAGE

#include "MessageHeader.hpp"

class RTMessage {
public:
    MessageHeader mh;

    char train;
    char src[6];
    char dest[6];

    RTMessage();

    int size();
};

#endif
