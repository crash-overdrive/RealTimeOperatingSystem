#include "Constants.hpp"
#include "user/message/SensorDiffMessage.hpp"

SensorDiffMessage::SensorDiffMessage() : mh(Constants::MSG::SENSOR_DIFF), count(0) {}

int SensorDiffMessage::size() {
    return sizeof(MessageHeader) + 4 - sizeof(MessageHeader) % 4 + sizeof(int) + count * sizeof(Sensor);
}

int SensorDiffMessage::maxSize() {
    return sizeof(MessageHeader) + 4 - sizeof(MessageHeader) % 4 + sizeof(int) + 80 * sizeof(Sensor);
}
