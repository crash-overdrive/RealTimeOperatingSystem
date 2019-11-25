#include "user/message/RTMessage.hpp"

RTMessage::RTMessage() : mh(Constants::MSG::RT), train(0) {}

int RTMessage::size() {
    return mh.size() + sizeof(char) + sizeof(char) * 6 + sizeof(char) * 6;
}
