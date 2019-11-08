#include "user/message/TimeMessage.hpp"

TimeMessage::TimeMessage() : mh(Constants::MSG::TIME) {}

int TimeMessage::size() {
    return mh.size() + sizeof(char)*3;
}
