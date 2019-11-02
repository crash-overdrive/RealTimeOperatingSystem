#include "user/message/TXMessage.hpp"

TXMessage::TXMessage() : mh(Constants::MSG::TX) {}

int TXMessage::size() {
    return mh.size() + sizeof(char);
}