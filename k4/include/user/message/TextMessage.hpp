#ifndef TEXT_MESSAGE
#define TEXT_MESSAGE

#include "../../Constants.hpp"
#include "MessageHeader.hpp"

class TextMessage {
public:
    MessageHeader mh;
    int msglen;
    char msg[Constants::MAX_MSGLEN];

    TextMessage();

    int size();
};

#endif