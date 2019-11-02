#ifndef TX_MESSAGE
#define TX_MESSAGE

#include "../../Constants.hpp"
#include "MessageHeader.hpp"

class TXMessage {
public:
    MessageHeader mh;
    char ch;

    TXMessage();

    int size();
};

#endif