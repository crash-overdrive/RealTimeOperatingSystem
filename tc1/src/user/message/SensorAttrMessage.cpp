#include "Constants.hpp"
#include "user/message/SensorAttrMessage.hpp"

SensorAttrMessage::SensorAttrMessage() : mh(Constants::MSG::SENSOR_ATTR), count(0) {}

int SensorAttrMessage::size() {
    return sizeof(MessageHeader) + 4 - sizeof(MessageHeader) % 4 + sizeof(int) + count * sizeof(SensorAttr);
}

int SensorAttrMessage::maxSize() {
    return sizeof(MessageHeader) + 4 - sizeof(MessageHeader) % 4 + sizeof(int) + 5 * sizeof(SensorAttr);
}
