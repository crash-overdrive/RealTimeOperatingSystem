#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "user/model/Location.hpp"
#include "user/model/Sensor.hpp"
#include "user/model/TrainInfo.hpp"

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

    int acc[15];
    int vel[15];
    int dec[15];
    Location location;
    Sensor nextSensor[2];
    TrainInfo trainInfo;
    int lastAttributionTime;

    bool updated;
    bool aheadOfPrediction[2];

    Train();
    Train(char n);
    static TRINDEX getTrainIndex(char number);
    static char getTrainNumber(char trainIndex);

    // TODO: update acceleration with correct values
    static const int accelerations[5][15];
    static const int velocities[5][15];
    static const int decelerations[5][15];
    static const int stoppingDistances[5][15];
};

#endif
