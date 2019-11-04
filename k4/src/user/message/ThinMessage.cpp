#include "user/message/ThinMessage.hpp"

ThinMessage::ThinMessage() {}
ThinMessage::ThinMessage(Constants::MSG type) : mh(type) {}

int ThinMessage::size() {
    return mh.size();
}