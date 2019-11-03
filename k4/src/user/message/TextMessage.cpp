#include "Constants.hpp"
#include "user/message/TextMessage.hpp"

TextMessage::TextMessage() : mh(Constants::MSG::TEXT), msglen(0) {}

int TextMessage::size() {
    return sizeof(MessageHeader) + sizeof(int) + msglen;
}