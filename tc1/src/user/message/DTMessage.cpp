#include "user/message/DTMessage.hpp"

DTMessage::DTMessage() : mh(Constants::MSG::DT), train(0) {}

int DTMessage::size() {
    return mh.size() + sizeof(char) * 7;
}
