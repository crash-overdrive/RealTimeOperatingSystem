#ifndef IDLE_MESSAGE
#define IDLE_MESSAGE

#include "../../Constants.hpp"
#include "MessageHeader.hpp"

class IdleMessage
{
public:
    MessageHeader mh;
    char integer;
    char fractional;

    IdleMessage();

    int size();
};

#endif
