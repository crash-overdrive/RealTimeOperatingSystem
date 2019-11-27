#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "user/model/Sensor.hpp"

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
    unsigned int position;
    Sensor nextSensor[2];

    Train();
    Train(char n);
    static TRINDEX getTrainIndex(char number);
    static char getTrainNumber(char trainIndex);
};

#endif
