#include "Constants.hpp"
#include "user/message/TrainMessage.hpp"

TrainMessage::TrainMessage() : mh(Constants::MSG::TRAIN), count{0} {}

int TrainMessage::size() {
    return sizeof(MessageHeader) + 4 - sizeof(MessageHeader) % 4 + sizeof(int) + sizeof(TrainInfo) * count;
}

int TrainMessage::maxSize() {
    return sizeof(MessageHeader) + 4 - sizeof(MessageHeader) % 4 + sizeof(int) + sizeof(TrainInfo) * 5;
}
