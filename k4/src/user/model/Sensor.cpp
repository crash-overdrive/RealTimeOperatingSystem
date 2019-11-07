#include "user/model/Sensor.hpp"

Sensor::Sensor() {
    bank = (char)0;
    number = (char)0;
}

Sensor::Sensor(char b, char n) {
    bank = b;
    number = n;
}
