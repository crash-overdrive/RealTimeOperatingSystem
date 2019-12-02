#include <cstring>

#include "Constants.hpp"
#include "Util.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/courier/TrainGUICourier.hpp"
#include "user/courier/TrainLocGUICourier.hpp"
#include "user/courier/TrainLocNavCourier.hpp"
#include "user/courier/TrainMarklinCourier.hpp"
#include "user/courier/TrainNavCourier.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/SensorAttrMessage.hpp"
#include "user/message/SensorDiffMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/message/TrainMessage.hpp"
#include "user/notifier/TrainNotifier.hpp"
#include "user/model/Train.hpp"
#include "user/server/TrainServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

void TrainServer::queueTrainSpeed(char tr, char s) {
    trbuf[Train::getTrainIndex(tr)].push(TRMessage(tr, s));
}

void TrainServer::queueReverse(char tr) {
    TRINDEX train = Train::getTrainIndex(tr);
    // TODO: calculate timing for slow down/ speed up
    char speed = trains[train].speed;
    trbuf[train].push(TRMessage(tr, (char)0));
    trbuf[train].push(TRMessage(tr, (char)15));
    trbuf[train].push(TRMessage(tr, speed));
}

// Returns the index of the first non-empty train command queue or -1 if all are empty
int TrainServer::queueIndex() {
    for (int i = 0; i < 5; ++i) {
        if (!trbuf[i].empty()) {
            return i;
        }
    }
    return -1;
}

TRMessage TrainServer::popTRMessage(int i) {
    return trbuf[i].pop();
}

void TrainServer::setTrainSpeed(char tr, char s) {
    trains[Train::getTrainIndex(tr)].speed = s;
}

void TrainServer::setTrainSpeed(int train, char s) {
    trains[train].speed = s;
}

void TrainServer::attributeSensors() {
    int attributionTime = Time(CLOCK);
    for (int i = 0; i < sdmsg->count; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (sdmsg->sensors[i] == trains[j].nextSensor[0]) {
                const TrackNode *currnode = &track.trackNodes[(int)trains[j].location.landmark];

                // Update train info
                trains[j].trainInfo.prev = trains[j].nextSensor[0];
                if (trains[j].aheadOfPrediction[0]) {
                    trains[j].trainInfo.distanceDelta = trains[j].location.offset/1000; // TODO: fix this, since we check if we're ahead of this sensor we know by how much
                    trains[j].aheadOfPrediction[0] = false;
                } else {
                    trains[j].trainInfo.distanceDelta = currnode->edges[getDirection(j)].dist - trains[j].location.offset/1000; // TODO: fix this too
                }
                trains[j].trainInfo.timeDelta = attributionTime - trains[j].lastAttributionTime - trains[j].trainInfo.predictedTime;
                trains[j].lastAttributionTime = attributionTime;

                // Update train location
                trains[j].location.landmark = Track::getIndex(trains[j].nextSensor[0]);
                trains[j].location.offset = 0;

                // Add to sensor attribution message
                samsg.sensorAttrs[samsg.count].sensor = sdmsg->sensors[i];
                samsg.sensorAttrs[samsg.count].train = trains[j].number;
                samsg.count++;

                // Clear the expected sensors
                trains[j].nextSensor[0].bank = 0;
                trains[j].nextSensor[0].number = 0;
                break;
            } else if (sdmsg->sensors[i] == trains[j].nextSensor[1]) {
                // TODO: handle updates determined by sensor attribution

                // Add to sensor attribution message
                samsg.sensorAttrs[samsg.count].sensor = sdmsg->sensors[i];
                samsg.sensorAttrs[samsg.count].train = trains[j].number;
                samsg.count++;

                // Clear the expected sensors
                trains[j].nextSensor[0].bank = 0;
                trains[j].nextSensor[0].number = 0;
                trains[j].nextSensor[1].bank = 0;
                trains[j].nextSensor[1].number = 0;
                break;
            }
        }
    }
}

void TrainServer::updatePredictions() {
    // TODO: determine what (if any) updates need to happen here
    // This is where we need to update before drawing
    for (int i = 0; i < spmsg->count; ++i) {
        int train = spmsg->predictions[i].train;
        int index = Train::getTrainIndex(train);

        // If this is our first prediction update, we need to set a location
        if (trains[index].location.landmark == 255) {
            trains[index].location.landmark = Track::getIndex(spmsg->predictions[i].nextSensor[0]);
            trains[index].location.offset = 0;
        }

        // Update expected sensors
        trains[index].nextSensor[0] = spmsg->predictions[i].nextSensor[0];
        trains[index].nextSensor[1] = spmsg->predictions[i].nextSensor[1];

        // Update prediction data
        trains[index].trainInfo.next = spmsg->predictions[i].nextSensor[0];
        trains[index].trainInfo.predictedDistance = spmsg->predictions[i].nextSensorDistance[0];
        trains[index].trainInfo.predictedTime = trains[index].trainInfo.predictedDistance/trains[index].vel[(int)trains[index].speed]; // TODO: determine what measurement of time is appropriate
        trains[index].updated = true;
    }
}

bool TrainServer::updated() {
    for (int i = 0; i < 5; ++i) {
        if (trains[i].updated) {
            return true;
        }
    }
    return false;
}

int TrainServer::getDirection(int i) {
    // TODO: Improve direction to account for sensor attributions
    if (trains[i].location.landmark == 255) {
        return DIR_STRAIGHT;
    }

    const TrackNode *currnode = &track.trackNodes[(int)trains[i].location.landmark];

    switch (track.trackNodes[(int)trains[i].location.landmark].type) {
        case NODE_SENSOR:
        case NODE_MERGE:
        case NODE_ENTER:
            break;
        case NODE_BRANCH:
            {
                const TrackNode *straight = track.trackNodes[(int)trains[i].location.landmark].edges[DIR_STRAIGHT].destNode;
                const TrackNode *curved = track.trackNodes[(int)trains[i].location.landmark].edges[DIR_CURVED].destNode;
                if (straight->type == NODE_TYPE::NODE_SENSOR && (straight->sensor == trains[i].nextSensor[0] || straight->sensor == trains[i].nextSensor[1])) {
                    return DIR_STRAIGHT;
                } else if (curved->type == NODE_TYPE::NODE_SENSOR && (curved->sensor == trains[i].nextSensor[0] || curved->sensor == trains[i].nextSensor[1])) {
                    return DIR_CURVED;
                } else if (track.trackNodes[(int)trains[i].location.landmark].num == 11) {
                    if (Sensor('A', 4) == trains[i].nextSensor[0]) {
                        return DIR_CURVED;
                    } else {
                        return DIR_STRAIGHT;
                    }
                } else if (straight->type == NODE_TYPE::NODE_SENSOR) {
                    return DIR_CURVED;
                } else if (curved->type == NODE_TYPE::NODE_SENSOR) {
                    return DIR_STRAIGHT;
                } else if (track.trackNodes[(int)trains[i].location.landmark].num == 153 || track.trackNodes[(int)trains[i].location.landmark].num == 155) {
                    return DIR_CURVED;
                } else {
                    bwprintf(COM2, "Train Server - Unexpected direction scenario from %s", track.trackNodes[(int)trains[i].location.landmark].name);
                }
            }
            break;
        case NODE_EXIT:
            bwprintf(COM2, "Train Server - Unexpected EXIT direction from %s", track.trackNodes[(int)trains[i].location.landmark].name);
            // bwprintf(COM2, "Train Server - Do we even support EXIT location updates? %d %d %s", trains[i].number, trains[i].location.landmark, track.trackNodes[(int)trains[i].location.landmark].name);
            SwitchOff();
            break;
        case NODE_NONE:
            bwprintf(COM2, "Train Server - Unexpected NONE direction from %s", track.trackNodes[(int)trains[i].location.landmark].name);
            // bwprintf(COM2, "Train Server - Do we even support NONE location updates? %d %d %s", trains[i].number, trains[i].location.landmark, track.trackNodes[(int)trains[i].location.landmark].name);
            SwitchOff();
            break;
        default:
            bwprintf(COM2, "Train Server - Unexpected NODE_TYPE %d received in getDirection() from %s with index %d", track.trackNodes[(int)trains[i].location.landmark].type, track.trackNodes[(int)trains[i].location.landmark].name, &track.trackNodes[(int)trains[i].location.landmark] - track.trackNodes);
            break;
    }
    return DIR_STRAIGHT;
}

void TrainServer::updateLocation() {
    prevtime = currtime;
    currtime = Time(CLOCK);
    int delta = currtime - prevtime; // Number of ticks, should always be 1
    ASSERT(delta > 0);
    for (int i = 0; i < 5; ++i) {
        // to stop updating locations, we need both speed and velocity to be zero
        if (trains[i].speed != 0 || trainVelocity[i] != 0) {
            // acceleration
            if (trainVelocity[i] < Train::velocities[i][trains[i].speed]) {
                trainVelocity[i] = trainVelocity[i] + Train::accelerations[i][trains[i].speed] * delta / 100;
                if (trainVelocity[i] > Train::velocities[i][trains[i].speed]) {
                    trainVelocity[i] = Train::velocities[i][trains[i].speed];
                }
                // bwprintf(COM2, "Accelerating %d ", trainVelocity[i]);
            }
            // deceleration
            else if (trainVelocity[i] > Train::velocities[i][trains[i].speed]) {
                trainVelocity[i] = trainVelocity[i] - Train::accelerations[i][trains[i].speed] * delta / 100;
                if (trainVelocity[i] < Train::velocities[i][trains[i].speed]) {
                    trainVelocity[i] = Train::velocities[i][trains[i].speed];
                }
                // bwprintf(COM2, "Decelerating %d ", trainVelocity[i]);
            } else {
                // bwprintf(COM2, "Constant Speed %d ", trainVelocity[i]);
            }

            // Calculate distance travelled since last tick
            int dist = trainVelocity[i] * delta / 100;
            // bwprintf(COM2, "Dist: %d\n\r", dist);
            // Update train location
            trains[i].location.offset += dist;
            int direction = DIR_AHEAD;
            direction = getDirection(i);
            dist = trains[i].location.offset - track.trackNodes[(int)trains[i].location.landmark].edges[direction].dist * 1000;
            while (dist >= 0) {
                int index = (track.trackNodes[(int)trains[i].location.landmark].edges[direction].destNode - track.trackNodes);
                trains[i].location.landmark = (char)index;
                trains[i].location.offset = dist;
                direction = getDirection(i);
                dist = trains[i].location.offset - track.trackNodes[(int)trains[i].location.landmark].edges[direction].dist * 1000;
            }

            // Check to see if we're ahead of our next predicted sensor
            if (!trains[i].aheadOfPrediction[0] && trains[i].location.landmark == Track::getIndex(trains[i].nextSensor[0])) {
                trains[i].aheadOfPrediction[0] = true;
            }

            // Add train to location message
            locmsg.locationInfo[locmsg.count].train = trains[i].number;
            locmsg.locationInfo[locmsg.count].location = trains[i].location;
            locmsg.count++;
        }
    }
}

void TrainServer::sendLocation() {
    if (locNavCourierReady && locmsg.count != 0) Reply(locNavCourier, (char *)&locmsg, locmsg.size());
    if (currtime % 10 == 0) {
        Reply(locGUICourier, (char *)&locmsg, locmsg.size());
    }
    locmsg.count = 0;
}

void TrainServer::sendGUI() {
    // TODO: need to know when train information has been updated
    if (updated() && guiCourierReady) {
        for (int i = 0; i < 5; ++i) {
            if (trains[i].updated) {
                trainmsg.trainInfo[trainmsg.count] = trains[i].trainInfo;
                trainmsg.count++;
                trains[i].updated = false;
            }
        }
        Reply(guiCourier, (char*)&trainmsg, trainmsg.size());
        guiCourierReady = false;
        trainmsg.count = 0;
    }
}

void TrainServer::init() {
    trains[T1] = Train(1);
    trains[T24] = Train(24);
    trains[T58] = Train(58);
    trains[T74] = Train(74);
    trains[T78] = Train(78);

    marklinCourier = Create(5, trainMarklinCourier);
    marklinCourierReady = false;
    navCourier = Create(5, trainNavCourier);
    navCourierReady = false;
    guiCourier = Create(8, trainGUICourier);
    guiCourierReady = false;
    notifier = Create(5, trainNotifier);
    locNavCourier = Create(5, trainLocNavCourier);
    locNavCourierReady = false;
    locGUICourier = Create(8, trainLocGUICourier);
    locGUICourierReady = false;


    // Set the speed measurements for the trains
    for (int i = 0; i < 5; ++i) {
        memcpy(trains[i].acc, Train::accelerations[i], 15*sizeof(int));
        memcpy(trains[i].vel, Train::velocities[i], 15*sizeof(int));
        memcpy(trains[i].dec, Train::decelerations[i], 15*sizeof(int));
    }

    prevtime = 0;
    currtime = 0;
    tickCount = 0;

    CLOCK = WhoIs("CLOCK SERVER");
}

void trainServer() {
    RegisterAs("TRAIN");

    TrainServer ts;
    ts.init();

    int result, tid;
    MessageHeader *mh = (MessageHeader*)&ts.msg;
    RVMessage *rvmsg = (RVMessage *)&ts.msg;
    TRMessage *trmsg = (TRMessage *)&ts.msg;

    ThinMessage rdymsg(Constants::MSG::RDY);
    ThinMessage errmsg(Constants::MSG::ERR);

    FOREVER {
        result = Receive(&tid, (char*)&ts.msg, 128);
        if(result < 0) {
            // TODO: handle empty message
        }

        if (mh->type == Constants::MSG::TICK) {
            ts.updateLocation();
            ts.sendLocation();
            Reply(tid, (char*)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::RDY) {
            int index = ts.queueIndex();
            if (tid == ts.marklinCourier) {
                if (index < 0) {
                    ts.marklinCourierReady = true;
                } else {
                    *trmsg = ts.popTRMessage(index);
                    ts.setTrainSpeed(index, trmsg->speed);
                    Reply(ts.marklinCourier, ts.msg, trmsg->size());
                }
            } else if (tid == ts.guiCourier) {
                ts.guiCourierReady = true;
                ts.sendGUI();
            } else if (tid == ts.locGUICourier) {
                ts.locGUICourierReady = true;
            } else if (tid == ts.locNavCourier) {
                ts.locNavCourierReady = true;
            } else {
                bwprintf(COM2, "Train Server - Unexpected ready message!");
            }
        } else if (mh->type == Constants::MSG::TR) {
            if (ts.marklinCourierReady) {
                ts.setTrainSpeed(trmsg->train, trmsg->speed);
                Reply(ts.marklinCourier, ts.msg, trmsg->size());
                ts.marklinCourierReady = false;
            } else {
                ts.queueTrainSpeed(trmsg->train, trmsg->speed);
            }
            Reply(tid, (char *)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::RV) {
            ts.queueReverse(rvmsg->train);
            if (ts.marklinCourierReady) {
                *trmsg = ts.popTRMessage(Train::getTrainIndex(rvmsg->train));
                Reply(ts.marklinCourier, ts.msg, trmsg->size());
                ts.marklinCourierReady = false;
            } else {
                ts.queueReverse(rvmsg->train);
            }
            Reply(tid, (char *)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::SENSOR_DIFF) {
            ts.attributeSensors();
            if (ts.samsg.count > 0) {
                Reply(ts.navCourier, (char *)&ts.samsg, ts.samsg.size());
                ts.navCourierReady = false;
            }
            Reply(tid, (char *)&ts.samsg, ts.samsg.size());
            ts.samsg.count = 0; // Reset the state of the attribution
        } else if (mh->type == Constants::MSG::SENSOR_PRED) {
            // If it's an empty message there's nothing to update (This occurs when receiving the first spmsg from the courier)
            if (ts.spmsg->count == 0) {
                ts.navCourierReady = true;
                continue;
            }
            ts.updatePredictions();
            if (tid == ts.navCourier) {
                // This is updated, but since this communication chain is dependent, do not actually need to track this
                ts.navCourierReady = true;
            } else {
                Reply(tid, (char*)&rdymsg, rdymsg.size());
            }
            ts.sendGUI(); // Send updated information to GUI Server
        } else {
            bwprintf(COM2, "Train Server - Unrecognized message type received %d", mh->type);
        }
    }
}
