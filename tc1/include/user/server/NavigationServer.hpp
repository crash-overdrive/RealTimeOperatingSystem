#ifndef NAVIGATION_SERVER
#define NAVIGATION_SERVER

#include "data-structures/Stack.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/RTMessage.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/SensorAttrMessage.hpp"
#include "user/message/SensorPredMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/trains/Track.hpp"

void navigationServer();

class NavigationServer {
    public:
        char msg[128];
        MessageHeader *mh = (MessageHeader*)&msg;
        RTMessage *rtmsg = (RTMessage*)&msg;
        SensorAttrMessage* samsg = (SensorAttrMessage*)&msg;
        SensorPredMessage spmsg;

        TRMessage trmsg;
        SWMessage swmsg;
        RVMessage rvmsg;

        int commandCourier;
        bool commandCourierReady;
        int trainCourier;
        bool trainCourierReady;

        Track track;

        DataStructures::Stack<int, TRACK_MAX> paths[5];
        DataStructures::Stack<Sensor, 20> sensorLists[5];

        void init();
        void predictSensors();
        void findPath();
        void navigate();
};

#endif
