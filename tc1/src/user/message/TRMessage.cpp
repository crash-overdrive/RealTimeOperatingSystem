#include "user/message/TRMessage.hpp"

TRMessage::TRMessage() : mh(Constants::MSG::TR), train(0), speed(0), headlights(false) {}

int TRMessage::size() {
    return mh.size() + sizeof(char) * 2 + sizeof(bool);
}