#include "user/message/IdleMessage.hpp"

IdleMessage::IdleMessage() : mh(Constants::MSG::IDLE) {}

int IdleMessage::size() {
    return mh.size() + sizeof(char)*2;
}
