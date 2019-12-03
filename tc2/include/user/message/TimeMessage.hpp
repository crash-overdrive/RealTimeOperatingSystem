#ifndef TIME_MESSAGE
#define TIME_MESSAGE

#include "../../Constants.hpp"
#include "MessageHeader.hpp"

class TimeMessage
{
public:
    MessageHeader mh;
    char m;
    char s;
    char ms;

    TimeMessage();

    int size();
};

#endif
