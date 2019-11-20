#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "user/model/Sensor.hpp"

class Train {
public:
    char number;
    char speed;
    bool headlights;
    bool reverse;

    unsigned int vel[15];
    unsigned int acc[15];
    unsigned int position;
    Sensor nextSensor[2];

    Train();
    Train(char n);
};

#endif
