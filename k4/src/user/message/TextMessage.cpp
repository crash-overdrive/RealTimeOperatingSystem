#include "Constants.hpp"
#include "user/message/TextMessage.hpp"

TextMessage::TextMessage() : mh(Constants::MSG::TEXT), msglen(0) {}

int TextMessage::size() {
    return sizeof(MessageHeader) + 4 - sizeof(MessageHeader) % 4 + sizeof(int) + msglen;
}

int TextMessage::maxSize() {
    return sizeof(MessageHeader) + 4 - sizeof(MessageHeader) % 4 + sizeof(int) + Constants::MAX_MSGLEN;
}
