#include "user/message/DelayMessage.hpp"

DelayMessage::DelayMessage() : mh(Constants::MSG::DELAY), delay{0} {}

int DelayMessage::size() {
    return mh.size() + 2 - mh.size() % 2 + sizeof(unsigned short);
}
