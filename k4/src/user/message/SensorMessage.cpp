#include "Constants.hpp"
#include "user/message/SensorMessage.hpp"
#include "user/model/Sensor.hpp"

SensorMessage::SensorMessage() : mh(Constants::MSG::SENSOR) {
    // Initialize these to be zero!
    for (int i = 0; i < 10; ++i) {
        sensorData[i].bank = 0;
        sensorData[i].number = 0;
    }
}

int SensorMessage::size() {
    return sizeof(MessageHeader) + 4 - sizeof(MessageHeader) % 4 + sizeof(Sensor)*10;
}