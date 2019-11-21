#include "Constants.hpp"
#include "user/message/SensorPredMessage.hpp"

SensorPredMessage::SensorPredMessage() : mh(Constants::MSG::SENSOR_PRED), count(0) {}

int SensorPredMessage::size() {
    return sizeof(MessageHeader) + 4 - sizeof(MessageHeader) % 4 + sizeof(int) + count * 2 * sizeof(SensorAttr);
}

int SensorPredMessage::maxSize() {
    return sizeof(MessageHeader) + 4 - sizeof(MessageHeader) % 4 + sizeof(int) + 20 * sizeof(SensorAttr);
}
