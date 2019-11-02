#include "user/message/MessageHeader.hpp"

MessageHeader::MessageHeader() {}

MessageHeader::MessageHeader(Constants::MSG type) : type{type} {}

int MessageHeader::size() {
    return sizeof(Constants::MSG);
}