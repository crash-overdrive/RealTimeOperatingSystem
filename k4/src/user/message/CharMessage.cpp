#include "user/message/CharMessage.hpp"

CharMessage::CharMessage() : mh(Constants::MSG::CHAR) {}

int CharMessage::size() {
    return mh.size() + sizeof(char);
}