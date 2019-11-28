#include "Constants.hpp"
#include "user/message/LocationMessage.hpp"

LocationMessage::LocationMessage() : mh(Constants::MSG::LOCATION), count(0) {}

int LocationMessage::size() {
    return sizeof(MessageHeader) + 4 - sizeof(MessageHeader) % 4 + sizeof(int) + count * sizeof(LocationInfo);
}

int LocationMessage::maxSize() {
    return sizeof(MessageHeader) + 4 - sizeof(MessageHeader) % 4 + sizeof(int) + 5 * sizeof(LocationInfo);
}
