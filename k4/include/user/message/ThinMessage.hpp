#ifndef THIN_MESSAGE
#define THIN_MESSAGE

#include "MessageHeader.hpp"

class ThinMessage {
public:
    MessageHeader mh;

    int size();
};

#endif