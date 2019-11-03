#ifndef CHAR_MESSAGE
#define CHAR_MESSAGE

#include "../../Constants.hpp"
#include "MessageHeader.hpp"

class CharMessage {
public:
    MessageHeader mh;
    char ch;

    CharMessage();

    int size();
};

#endif