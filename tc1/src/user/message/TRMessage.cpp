#include "user/message/TRMessage.hpp"

TRMessage::TRMessage() : mh(Constants::MSG::TR), train{0}, speed{0}, headlights{true} {}
TRMessage::TRMessage(char tr, char s) : mh(Constants::MSG::TR), train{tr}, speed{s}, headlights{true} {}

int TRMessage::size() {
    return mh.size() + sizeof(char) * 2 + sizeof(bool);
}
