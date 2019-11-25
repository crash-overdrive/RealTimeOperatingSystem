#include "Constants.hpp"
#include "user/message/TrainMessage.hpp"
#include "user/model/Sensor.hpp"

TrainMessage::TrainMessage() : mh(Constants::MSG::TRAIN), predTime{0}, predDist{0}, realTime{0}, realDist{0} {}

int TrainMessage::size() {
    return sizeof(MessageHeader) + 4 - sizeof(MessageHeader) % 4 + sizeof(Sensor) * 2 + sizeof(int) * 4;
}
