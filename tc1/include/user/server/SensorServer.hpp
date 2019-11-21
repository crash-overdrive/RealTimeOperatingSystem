#ifndef SENSOR_SERVER_HPP
#define SENSOR_SERVER_HPP

#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/SensorMessage.hpp"
#include "user/message/SensorAttrMessage.hpp"
#include "user/message/SensorDiffMessage.hpp"
#include "user/message/TextMessage.hpp"
#include "user/model/Sensor.hpp"

void sensorServer();

class SensorServer {
public:
    char msg[128];
    MessageHeader *mh = (MessageHeader*)&msg;
    TextMessage *textmsg = (TextMessage *)&msg;
    SensorAttrMessage *samsg = (SensorAttrMessage *)&msg;
    SensorMessage sensormsg;
    SensorDiffMessage sdmsg;
    // SensorDiffMessage *sdmsg = (SensorDiffMessage *)&msg;
    DataStructures::RingBuffer<SensorAttr, Constants::SensorData::NUM_SENSORS_DISPLAYED> sensorHistory;

    int trainCourier;
    int guiCourier;
    bool trainCourierReady;
    bool guiCourierReady;
    bool sensorHistoryUpdated;
    // DataStructures::RingBuffer<SWMessage, 32> swbuf;
    // DataStructures::RingBuffer<SWMessage, 32> guibuf;

    void init();
    void sendTrain();
    void sendGUI();
    char getSensorNumber(int bank, int bit);
    char getSensorBank(int bank);
    void processData();
    void updateHistory();
};

#endif
