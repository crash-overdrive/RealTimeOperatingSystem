#ifndef THIN_MESSAGE
#define THIN_MESSAGE

#include "MessageHeader.hpp"

class ThinMessage {
public:
    MessageHeader mh;

    ThinMessage();
    ThinMessage(Constants::MSG type);

    int size();
};

#endif