#ifndef DT_MESSAGE
#define DT_MESSAGE

#include "MessageHeader.hpp"

class DTMessage {
public:
    MessageHeader mh;

    char train;
    char dest[6];

    DTMessage();

    int size();
};

#endif
