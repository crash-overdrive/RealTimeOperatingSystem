#ifndef SENSOR_HPP
#define SENSOR_HPP

class Sensor {
public:
    char bank;
    char number;

    Sensor();
    Sensor(char b, char n);

    bool operator==(const Sensor& other) const;
    bool operator!=(const Sensor& other) const;
};

// inline bool operator==(const Sensor& lhs, const Sensor& rhs) {}

class SensorAttr {
public:
    Sensor sensor;
    char train;

    SensorAttr();
    SensorAttr(char b, char n, char t);
};

class SensorPred {
public:
    Sensor nextSensor[2];
    int nextSensorDistance[2];
    char train;

    SensorPred();
};

#endif
