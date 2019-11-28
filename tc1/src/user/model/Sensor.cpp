#include "user/model/Sensor.hpp"

Sensor::Sensor() : bank{0}, number{0} {}

Sensor::Sensor(char b, char n) : bank{b}, number{n} {}

bool Sensor::operator==(const Sensor& other) const {
    return bank == other.bank && number == other.number;
}

SensorAttr::SensorAttr() : sensor{0, 0}, train{0} {}

SensorAttr::SensorAttr(char b, char n, char t) : sensor{b, n}, train{t} {}

SensorPred::SensorPred() { train = 0; }
