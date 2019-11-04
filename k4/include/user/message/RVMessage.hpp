#ifndef RV_MESSAGE
#define RV_MESSAGE

#include "MessageHeader.hpp"

class RVMessage {
public:
    MessageHeader mh;

    char train;

    RVMessage();

    int size();
};

#endif