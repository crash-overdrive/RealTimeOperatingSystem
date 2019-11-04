#include "user/message/SWMessage.hpp"

SWMessage::SWMessage() : mh(Constants::MSG::SW), sw(0), state(0) {}

int SWMessage::size() {
    return mh.size() + sizeof(char) * 2;
}