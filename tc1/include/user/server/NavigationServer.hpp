#ifndef NAVIGATION_SERVER
#define NAVIGATION_SERVER

#include "data-structures/Stack.hpp"
#include "data-structures/RingBuffer.hpp"
#include "user/message/LocationMessage.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/RTMessage.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/SensorAttrMessage.hpp"
#include "user/message/SensorPredMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/model/Track.hpp"

enum COMMANDS {
    TRAIN,
    SWITCH,
    REVERSE,
    NONE,
};

class TR {
    public:
        char train;
        char speed;
};
class SW {
    public:
        char number;
        char orientation;
};

class RV {
    public:
        char train;
};

union TR_SW_RV {
    TR tr;
    RV rv;
    SW sw;
};

class TrackCommand {
    public:
        TR_SW_RV tr_sw_rv;
        int type;
};

class NavigationServer {
    public:
        char msg[128];
        MessageHeader *mh = (MessageHeader*)&msg;
        RTMessage *rtmsg = (RTMessage*)&msg;
        SensorAttrMessage* samsg = (SensorAttrMessage*)&msg;
        LocationMessage* locmsg = (LocationMessage*) &msg;
        SensorPredMessage spmsg;

        TRMessage trmsg;
        SWMessage swmsg;
        RVMessage rvmsg;

        int commandCourier;
        bool commandCourierReady;
        int trainCourier;
        bool trainCourierReady;

        Track track;

        bool stopCommandSent[5] = {false};
        Location lastKnownLocations[5];
        DataStructures::Stack<int, TRACK_MAX> paths[5];
        DataStructures::Stack<int, 40> landmarkDistanceLists[5];
        DataStructures::Stack<int, 40> sensorLists[5];
        DataStructures::Stack<int, 40> sensorDistanceLists[5];
        DataStructures::Stack<int, 40> reverseList[5];
        DataStructures::Stack<SW, 40> branchList[5];
        DataStructures::Stack<int, 40> reservationsList;

        DataStructures::RingBuffer<TrackCommand, 40> cmdqueue;

        void init();
        void transmitToTrainServer(int msgType);
        void queueCommand(TrackCommand trackCommand); // queues messages to be sent to Command Server
        void issueCommand(); // issues command to command server
        void reserveTrack(); // reserves whatever is in reservationList
        void unreserveTrack(int trackIndex); // unreserves TrackNode at trackIndex

        void predictSensors(); // only called when we get SENSOR_ATTRIBUTE msg, pops sensor List, sensorDistance List path and landmarkDistance List
        void initSensorPredictions(); // only called when findPath() was successful
        bool findPath(); // only called when we get RT msg
        int journeyLeft(int trainIndex); // returns journey left for travel, assumes path is matched up to lastKnownLocation
        void evaluate(int trainIndex); // evaluates next 300mm of path for train, calls queueCommand appropriately
        void navigate(); // only called when we get LOCATION msg

        // int journeyLeft(int trainIndex, int trackIndex, int offset);
};

void navigationServer();

#endif
