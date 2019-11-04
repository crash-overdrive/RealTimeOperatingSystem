#include "user/message/RVMessage.hpp"

RVMessage::RVMessage() : mh(Constants::MSG::RV), train(0) {}

int RVMessage::size() {
    return mh.size() + sizeof(char);
}