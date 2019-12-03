#include "user/message/RVMessage.hpp"

RVMessage::RVMessage() : mh(Constants::MSG::RV), train(0), speed(0) {}

int RVMessage::size() {
    return mh.size() + sizeof(char)*2;
}