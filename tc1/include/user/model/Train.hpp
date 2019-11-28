#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "user/model/Sensor.hpp"
#include "user/model/Location.hpp"

enum TRINDEX {
    T1 = 0,
    T24,
    T58,
    T74,
    T78,
};

class Train {
public:
    char number;
    char speed;
    bool headlights;
    bool reverse;

    unsigned int vel[15];
    unsigned int acc[15];
    Location location;
    Sensor nextSensor[2];

    bool updated;
    bool locationUpdated; // Do we need this?

    Train();
    Train(char n);
    static TRINDEX getTrainIndex(char number);
    static char getTrainNumber(char trainIndex);
};

#endif
