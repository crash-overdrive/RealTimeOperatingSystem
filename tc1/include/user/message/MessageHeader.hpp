#ifndef MESSAGE_HEADER
#define MESSAGE_HEADER

#include "../../Constants.hpp"

class MessageHeader {
public:
    Constants::MSG type;

    MessageHeader();
    MessageHeader(Constants::MSG type);

    int size();
};

#endif