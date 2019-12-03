#include "user/message/TPMessage.hpp"

TPMessage::TPMessage() : mh(Constants::MSG::TP), train(0) {}

int TPMessage::size() {
    return mh.size() + sizeof(char) * 7;
}
