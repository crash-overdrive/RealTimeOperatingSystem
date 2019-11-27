#ifndef TRAIN_INFO_HPP
#define TRAIN_INFO_HPP

#include "user/model/Sensor.hpp"

class TrainInfo {
public:
    char number;
    char speed;

    Sensor next;
    Sensor prev;
    int predictedTime, predictedDist, realTime, realDist;

    TrainInfo();
};

#endif
