#include <string.h>

#include "Constants.hpp"
#include "Util.hpp"
#include "data-structures/RingBuffer.hpp"
#include "data-structures/Stack.hpp"
#include "io/bwio.hpp"
#include "io/StringFormat.hpp"
#include "user/courier/NavCSCourier.hpp"
#include "user/courier/NavTrainCourier.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/DTMessage.hpp"
#include "user/message/RTMessage.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/SensorAttrMessage.hpp"
#include "user/message/SensorPredMessage.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TPMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/model/Train.hpp"
#include "user/server/NavigationServer.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "user/model/Track.hpp"

#define FOREVER for(;;)

constexpr int reverseClearance = 250;

int convertToIndex(Sensor sensor) {
    return sensor.bank - 'A' * 16 + sensor.bank - 1;
}

Sensor convertToSensor(int trackIndex) {
    return Sensor(trackIndex / 16 + 'A',trackIndex % 16 + 1);
}

int nextNodeToBeConsidered(const Track* track, bool visitedNodes[], int dist[]) {
    int noOfNodes = track->noOfNodes;
    int i = 0;
    while ((visitedNodes[i] == true || track->trackNodes[i].reserved == true) && i < noOfNodes) {
        ++i;
    }
    if (i >= noOfNodes) {
        return -1;
    }

    int minIndex = i;
    int minValue = dist[i];

    ++i;

    for (; i < noOfNodes; ++i) {
        if (visitedNodes[i] == false && track->trackNodes[i].reserved == false && dist[i] < minValue) {
            minValue = dist[i];
            minIndex = i;
        }
    }
    return minIndex;
}

bool djikstraFinished(const Track* track, bool visitedNodes[]) {
    int noOfNodes = track->noOfNodes;
    for (int i = 0; i < noOfNodes; ++i) {
        if (visitedNodes[i] == false && track->trackNodes[i].reserved == false) {
            return false;
        }
    }
    return true;
}

void djikstra(const Track* track, int srcIndex, int destIndex, int lastHop[]) {
    int dist[track->noOfNodes];
    bool visitedNodes[track->noOfNodes];

    for (int i = 0; i < track->noOfNodes; ++i) {
        dist[i] = __INT_MAX__;
        lastHop[i] = -1;
        visitedNodes[i] = false;
    }

    dist[srcIndex] = 0;
    visitedNodes[srcIndex] = true;

    switch(track->trackNodes[srcIndex].type) {
        case NODE_TYPE::NODE_SENSOR:
        {
            if (track->trackNodes[srcIndex].reserved) {
                bwprintf(COM2, "Navigation Server - Djikstra failed, starting point %s reserved\n\r", track->trackNodes[srcIndex].name);
                return;
            }

            int trackIndexAhead = track->trackNodes[srcIndex].edges[DIR_AHEAD].destNode - &track->trackNodes[0];
            if (!track->trackNodes[trackIndexAhead].reserved) {
                dist[trackIndexAhead] = track->trackNodes[srcIndex].edges[DIR_AHEAD].dist;
                lastHop[trackIndexAhead] = srcIndex;
            } else {
                bwprintf(COM2, "Navigation Server - Djikstra initiliaztion in ahead direction failed %s reserved\n\r", track->trackNodes[trackIndexAhead].name);
            }

            int trackIndexReverse = track->trackNodes[srcIndex].reverseNode - &track->trackNodes[0];
            if (!track->trackNodes[trackIndexReverse].reserved) {
                dist[trackIndexReverse] = 0;
                lastHop[trackIndexReverse] = srcIndex;
            } else {
                bwprintf(COM2, "Navigation Server - Djikstra initiliaztion in reverse direction failed %s reserved\n\r", track->trackNodes[trackIndexReverse].name);
            }
            break;
        }
        default:
            bwprintf(COM2, "Navigation Server - Invalid starting location type\n\r");
            return;
    }

    do {
        int minVertexNotVisited = nextNodeToBeConsidered(track, visitedNodes, dist);
        if (minVertexNotVisited == -1) {
            bwprintf(COM2, "Navigation Server - minVertexNotVisited is -1...\n\r");
            return;
        }
        visitedNodes[minVertexNotVisited] = true;

        switch(track->trackNodes[minVertexNotVisited].type) {
            case NODE_TYPE::NODE_SENSOR:
            {
                int trackIndexAhead = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].destNode - &track->trackNodes[0];
                if (visitedNodes[trackIndexAhead] == false && !track->trackNodes[trackIndexAhead].reserved) {
                    int distToNeighbour = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].dist;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[trackIndexAhead]) {
                        dist[trackIndexAhead] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[trackIndexAhead] = minVertexNotVisited;
                    }
                }
                int trackIndexReverse = track->trackNodes[minVertexNotVisited].reverseNode - &track->trackNodes[0];
                if (visitedNodes[trackIndexReverse] == false && !track->trackNodes[trackIndexReverse].reserved) {
                    int distToNeighbour = 0;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[trackIndexReverse]) {
                        dist[trackIndexReverse] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[trackIndexReverse] = minVertexNotVisited;
                    }
                }
                break;
            }

            case NODE_TYPE::NODE_BRANCH:
            {
                int trackIndexStraight = track->trackNodes[minVertexNotVisited].edges[DIR_STRAIGHT].destNode - &track->trackNodes[0];
                if (visitedNodes[trackIndexStraight] == false && !track->trackNodes[trackIndexStraight].reserved) {
                    int distToNeighbour = track->trackNodes[minVertexNotVisited].edges[DIR_STRAIGHT].dist;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[trackIndexStraight]) {
                        dist[trackIndexStraight] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[trackIndexStraight] = minVertexNotVisited;
                    }
                }
                int trackIndexCurved = track->trackNodes[minVertexNotVisited].edges[DIR_CURVED].destNode - &track->trackNodes[0];
                if (visitedNodes[trackIndexCurved] == false && !track->trackNodes[trackIndexCurved].reserved) {
                    int distToNeighbour = track->trackNodes[minVertexNotVisited].edges[DIR_CURVED].dist;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[trackIndexCurved]) {
                        dist[trackIndexCurved] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[trackIndexCurved] = minVertexNotVisited;
                    }
                }
                int trackIndexReverse = track->trackNodes[minVertexNotVisited].reverseNode - &track->trackNodes[0];
                if (visitedNodes[trackIndexReverse] == false && !track->trackNodes[trackIndexReverse].reserved) {
                    int distToNeighbour = 0;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[trackIndexReverse]) {
                        dist[trackIndexReverse] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[trackIndexReverse] = minVertexNotVisited;
                    }
                }
                break;
            }

            case NODE_TYPE::NODE_MERGE:
            {
                int trackIndexAhead = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].destNode - &track->trackNodes[0];
                if (visitedNodes[trackIndexAhead] == false && !track->trackNodes[trackIndexAhead].reserved) {
                    int distToNeighbour = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].dist;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[trackIndexAhead]) {
                        dist[trackIndexAhead] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[trackIndexAhead] = minVertexNotVisited;
                    }
                }
                int trackIndexReverse = track->trackNodes[minVertexNotVisited].reverseNode - &track->trackNodes[0];
                if (visitedNodes[trackIndexReverse] == false && !track->trackNodes[trackIndexReverse].reserved) {
                    int distToNeighbour = 0;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[trackIndexReverse]) {
                        dist[trackIndexReverse] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[trackIndexReverse] = minVertexNotVisited;
                    }
                }
                break;
            }

            case NODE_TYPE::NODE_ENTER:
            {
                int trackIndexAhead = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].destNode - &track->trackNodes[0];
                if (visitedNodes[trackIndexAhead] == false && !track->trackNodes[trackIndexAhead].reserved) {
                    int distToNeighbour = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].dist;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[trackIndexAhead]) {
                        dist[trackIndexAhead] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[trackIndexAhead] = minVertexNotVisited;
                    }
                }
                break;
            }

            case NODE_TYPE::NODE_EXIT:
            {
                int trackIndexReverse = track->trackNodes[minVertexNotVisited].reverseNode - &track->trackNodes[0];
                if (visitedNodes[trackIndexReverse] == false && !track->trackNodes[trackIndexReverse].reserved) {
                    int distToNeighbour = 0;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[trackIndexReverse]) {
                        dist[trackIndexReverse] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[trackIndexReverse] = minVertexNotVisited;
                    }
                }
                break;
            }
            default: {
                break;
            }
        }
    } while(!djikstraFinished(track, visitedNodes));
}

void NavigationServer::freeReservationsForTrain(int trainIndex) {
    while(!freeReservationsList[trainIndex].empty()) {
        int trackIndex = freeReservationsList[trainIndex].pop();
        track.trackNodes[trackIndex].reserved = false;
        track.trackNodes[trackIndex].reverseNode->reserved = false;
    }
}

void NavigationServer::reserveTrack() {
    while(!reservationsList.empty()) {
        int trackIndex = reservationsList.pop();
        track.trackNodes[trackIndex].reserved = true;
        track.trackNodes[trackIndex].reverseNode->reserved = true;
    }
}

void NavigationServer::transmitToCommandServer(int msgType) {
    ASSERT(commandCourierReady = true);
    commandCourierReady = false;
    switch (msgType) {
        case Constants::MSG::TR:
            Reply(commandCourier, (char*)&trmsg, trmsg.size());
            break;
        case Constants::MSG::RV:
            Reply(commandCourier, (char*)&rvmsg, rvmsg.size());
            break;
        case Constants::MSG::SW:
            Reply(commandCourier, (char*)&swmsg, swmsg.size());
            break;
        default:
            commandCourierReady = true;
            bwprintf(COM2, "Navigation Server - Tried to send invalid msg to Command Server: %d", msgType);
            break;
    }
}

void NavigationServer::transmitToTrainServer(int msgType) {
    ASSERT(trainCourierReady == true);
    if (msgType == Constants::MSG::SENSOR_PRED) {
        trainCourierReady = false;
        Reply(trainCourier, (char*)&spmsg, spmsg.size());
    } else {
        bwprintf(COM2, "Navigation Server - Tried to send invalid msg to Train Server: %d", msgType);
    }
}

bool NavigationServer::findPath() {
    int srcIndex = -1;
    int destIndex = -1;
    int lastHop[track.noOfNodes];

    for(int i = 0; i < track.noOfNodes; ++i) {
        if (!strcmp(track.trackNodes[i].name, rtmsg->src)) {
            srcIndex = i;
        } else if(!strcmp(track.trackNodes[i].name, rtmsg->dest)) {
            destIndex = i;
        }
    }

    if (srcIndex == -1 || destIndex == -1) {
        bwprintf(COM2, "Navigation Server - Bad src %s %d  or dest %s %d\n\r", rtmsg->src, srcIndex, rtmsg->dest, destIndex);
        return false;
    }

    djikstra(&track, srcIndex, destIndex, lastHop);

    int nextTrackIndex = -1;
    int currentTrackIndex = -1;
    int distance = 0;
    int trainIndex = Train::getTrainIndex(rtmsg->train);

    do {
        nextTrackIndex = currentTrackIndex;
        currentTrackIndex = currentTrackIndex == -1 ? destIndex : lastHop[currentTrackIndex];

        // error checking
        if (currentTrackIndex == -1) {
            bwprintf(COM2, "Navigation Server - No Path Exists from %s to %s\n\r", rtmsg->src, rtmsg->dest); // TODO(sgaweda): This actually shouldn't never happen and if it can we have to find a way around it!
            paths[trainIndex].reset();
            reservationsList.reset();
            sensorLists[trainIndex].reset();
            sensorDistanceLists[trainIndex].reset();
            return false;
        }

        // push to sensorDistance List
        if (nextTrackIndex == -1) {
            sensorDistanceLists[trainIndex].push(distance);
            bwprintf(COM2, "Pushed %d for %s\n\r", distance, track.trackNodes[currentTrackIndex].name);
        } else {
            switch (track.trackNodes[currentTrackIndex].type) {
                case NODE_TYPE::NODE_SENSOR:
                {
                    int trackIndexAhead = track.trackNodes[currentTrackIndex].edges[DIR_AHEAD].destNode - &track.trackNodes[0];
                    int trackIndexReverse = track.trackNodes[currentTrackIndex].reverseNode - &track.trackNodes[0];
                    if (trackIndexAhead == nextTrackIndex) {
                        distance += track.trackNodes[currentTrackIndex].edges[DIR_AHEAD].dist;
                    } else if (trackIndexReverse == nextTrackIndex) {
                        distance += reverseClearance;
                    } else {
                        bwprintf(COM2, "Navigation Server - Error initializing Sensor distance list, node sensor\n\r");
                    }
                    sensorDistanceLists[trainIndex].push(distance);
                    bwprintf(COM2, "Pushed %d for %s\n\r", distance, track.trackNodes[currentTrackIndex].name);
                    distance = 0;
                    break;
                }
                case NODE_TYPE::NODE_BRANCH:
                {
                    int trackIndexStraight = track.trackNodes[currentTrackIndex].edges[DIR_STRAIGHT].destNode - &track.trackNodes[0];
                    int trackIndexCurved = track.trackNodes[currentTrackIndex].edges[DIR_CURVED].destNode - &track.trackNodes[0];
                    if (trackIndexStraight == nextTrackIndex) {
                        distance += track.trackNodes[currentTrackIndex].edges[DIR_STRAIGHT].dist;
                    } else if (trackIndexCurved == nextTrackIndex) {
                        distance += track.trackNodes[currentTrackIndex].edges[DIR_CURVED].dist;
                    } else {
                        bwprintf(COM2, "Navigation Server - Error initializing Sensor distance list, branch selection\n\r");
                    }
                    break;
                }
                case NODE_TYPE::NODE_MERGE:
                {
                    int trackIndexAhead = track.trackNodes[currentTrackIndex].edges[DIR_AHEAD].destNode - &track.trackNodes[0];
                    int trackIndexReverse = track.trackNodes[currentTrackIndex].reverseNode - &track.trackNodes[0];
                    if (trackIndexAhead == nextTrackIndex) {
                        distance += track.trackNodes[currentTrackIndex].edges[DIR_AHEAD].dist;
                    } else if (trackIndexReverse == nextTrackIndex) {
                        distance += reverseClearance;
                    } else {
                        bwprintf(COM2, "Navigation Server - Error initializing Sensor distance list, node merge\n\r");
                    }
                    break;
                }
                case NODE_TYPE::NODE_ENTER:
                {
                    int trackIndexAhead = track.trackNodes[currentTrackIndex].edges[DIR_AHEAD].destNode - &track.trackNodes[0];
                    if (trackIndexAhead == nextTrackIndex) {
                        distance += track.trackNodes[currentTrackIndex].edges[DIR_AHEAD].dist;
                    } else {
                        bwprintf(COM2, "Navigation Server - Error initializing Sensor distance list, node enter\n\r");
                    }
                    break;
                }
                case NODE_TYPE::NODE_EXIT:
                {
                    int trackIndexReverse = track.trackNodes[currentTrackIndex].reverseNode - &track.trackNodes[0];
                    if (trackIndexReverse == nextTrackIndex) {
                        distance += reverseClearance;
                    } else {
                        bwprintf(COM2, "Navigation Server - Error initializing Sensor distance list, node exit\n\r");
                    }
                    break;
                }
                default:
                {
                    bwprintf(COM2, "Navigation Server - Error initializing sensorDistanceList\n\r");
                    break;
                }
            }
        }

        // push to Sensor List
        if (track.trackNodes[currentTrackIndex].type == NODE_SENSOR) {
            sensorLists[trainIndex].push(convertToSensor(currentTrackIndex));
        }

        // push to paths List
        paths[trainIndex].push(currentTrackIndex);

        //push to reservationList
        reservationsList.push(currentTrackIndex);

    } while(currentTrackIndex != srcIndex);
    ASSERT(sensorLists[trainIndex].size() == sensorDistanceLists[trainIndex].size());

    // reserve Track
    reserveTrack();
    // push start marker to path
    paths[trainIndex].push(-1);
    return true;
}

void NavigationServer::initSensorPredictions() {
    int trainIndex = Train::getTrainIndex(rtmsg->train);
    spmsg.predictions[spmsg.count].nextSensor[0] = sensorLists[trainIndex].peek();
    spmsg.predictions[spmsg.count].nextSensor[1] = sensorLists[trainIndex].peekSecond();
    spmsg.predictions[spmsg.count].nextSensorDistance[0] = sensorDistanceLists[trainIndex].peek();
    spmsg.predictions[spmsg.count].nextSensorDistance[1] = sensorDistanceLists[trainIndex].peekSecond();
    spmsg.predictions[spmsg.count].train = rtmsg->train;
    spmsg.count++;
    transmitToTrainServer(Constants::MSG::SENSOR_PRED);
    spmsg.count = 0;
}

void NavigationServer::predictSensors() {
    for (int i = 0; i < samsg->count; ++i) {
        char train = samsg->sensorAttrs[i].train;
        int trainIndex = Train::getTrainIndex(train);
        if (sensorLists[trainIndex].peek() == samsg->sensorAttrs[i].sensor) {
            ASSERT(samsg->sensorAttrs[i].sensor == convertToSensor(paths[trainIndex].peek()));

            int sensorIndex = paths[trainIndex].pop();
            freeReservationsList[trainIndex].push(sensorIndex);
            freeReservationsForTrain(trainIndex);

            sensorLists[trainIndex].pop();
            sensorDistanceLists[trainIndex].pop();
            spmsg.predictions[spmsg.count].nextSensor[0] = sensorLists[trainIndex].peek();
            spmsg.predictions[spmsg.count].nextSensor[1] = sensorLists[trainIndex].peekSecond();
            spmsg.predictions[spmsg.count].nextSensorDistance[0] = sensorDistanceLists[trainIndex].peek();
            spmsg.predictions[spmsg.count].nextSensorDistance[1] = sensorDistanceLists[trainIndex].peekSecond();
            spmsg.predictions[spmsg.count].train = train;
            spmsg.count++;
        } else if (sensorLists[trainIndex].peekSecond() == samsg->sensorAttrs->sensor) {
            // TODO: handle recovery logic if needed
            Sensor sensor1 = sensorLists[trainIndex].pop();
            int sensor1Index = paths[trainIndex].pop();
            ASSERT((sensor1 == convertToSensor(sensor1Index)));
            freeReservationsList[trainIndex].push(sensor1Index);

            Sensor sensor2 = sensorLists[trainIndex].pop();
            int trackIndex = -1;
            do {
                trackIndex = paths[trainIndex].pop();
                freeReservationsList[trainIndex].push(trackIndex);
            } while (track.trackNodes[trackIndex].type != NODE_TYPE::NODE_SENSOR);
            ASSERT((sensor2 == convertToSensor(trackIndex)));
            freeReservationsForTrain(trainIndex);

            sensorDistanceLists[trainIndex].pop();
            sensorDistanceLists[trainIndex].pop();

            spmsg.predictions[spmsg.count].nextSensor[0] = sensorLists[trainIndex].peek();
            spmsg.predictions[spmsg.count].nextSensor[1] = sensorLists[trainIndex].peekSecond();
            spmsg.predictions[spmsg.count].nextSensorDistance[0] = sensorDistanceLists[trainIndex].peek();
            spmsg.predictions[spmsg.count].nextSensorDistance[1] = sensorDistanceLists[trainIndex].peekSecond();
            spmsg.predictions[spmsg.count].train = train;
            spmsg.count++;
        } else {
            bwprintf(COM2, "Nav Server - Bad Sensor Attribution.."); // NOTE(sgaweda): This should NEVER happen, if a train attributes a sensor incorrectly, it's because the Navigation Server told it to!
        }



        // TODO: refactor this logic out of here
        if (sensorLists[trainIndex].peek().bank == 0) {
            // STOP TRAIN
            trmsg.speed = 0;
            trmsg.train = train;
            transmitToCommandServer(Constants::MSG::TR);
        } else if (sensorLists[trainIndex].peekSecond().bank == 0) {
            // SLOW TRAIN
            trmsg.speed = 8;
            trmsg.train = train;
            transmitToCommandServer(Constants::MSG::TR);
        }



    }
}

void NavigationServer::navigate() {
    ASSERT(commandCourierReady == true);
    for (int trainIndex = 0; trainIndex < 5; ++trainIndex) {
        // Do nothing if there's no path
        if (paths[trainIndex].empty()) {
            continue;
        }

        int trackIndex = paths[trainIndex].peek();

        if (trackIndex == -1) { //start the train
            paths[trainIndex].pop();
            int trainNumber = Train::getTrainNumber(trainIndex);
            trmsg.train = trainNumber;
            trmsg.speed = 14;
            transmitToCommandServer(Constants::MSG::TR);
            return;
        }

        const TrackNode* currentLandmark = &track.trackNodes[trackIndex];
        const char* currentLandmarkName = currentLandmark->name; // TODO: Are you sure this works the way you think it does? How the does while loop know how to allocate space for each string? This looks wrong, and you're STILL ignoring warnings!!!

        if (currentLandmark->type == NODE_TYPE::NODE_SENSOR) {
            //TODO: implement me for reversing
            continue;
        }

        while(currentLandmark->type != NODE_TYPE::NODE_SENSOR) {
            int landmarkIndex = paths[trainIndex].pop();
            freeReservationsList[trainIndex].push(landmarkIndex);
            ASSERT(paths[trainIndex].empty() == false);
            const TrackNode* nextLandmark =  &track.trackNodes[paths[trainIndex].peek()];
            const char* nextLandmarkName = nextLandmark->name;

            switch (currentLandmark->type) {
                case NODE_TYPE::NODE_BRANCH:
                {
                    const char* straightDirectionLandmarkName = currentLandmark->edges[DIR_STRAIGHT].destNode->name;
                    const char* curvedDirectionLandmarkName = currentLandmark->edges[DIR_CURVED].destNode->name;

                    if (!strcmp(nextLandmarkName, straightDirectionLandmarkName)) {
                        swmsg.sw = currentLandmark->num;
                        swmsg.state = 'S';
                        transmitToCommandServer(Constants::MSG::SW);
                        return;
                    } else if (!strcmp(nextLandmarkName, curvedDirectionLandmarkName)) {
                        swmsg.sw = currentLandmark->num;
                        swmsg.state = 'C';
                        transmitToCommandServer(Constants::MSG::SW);
                        return;
                    } else {
                        bwprintf(COM2, "Navigation Server - Branch direction selection failed!\n\r");
                    }
                    break;
                }
                case NODE_TYPE::NODE_MERGE:
                {
                    const char* aheadDirectionLandmarkName = currentLandmark->edges[DIR_AHEAD].destNode->name;
                    const char* reverseDirectionLandmarkName = currentLandmark->reverseNode->name;
                    if (!strcmp(nextLandmarkName, aheadDirectionLandmarkName)) {
                    } else if(!strcmp(nextLandmarkName, reverseDirectionLandmarkName)) {
                        rvmsg.train = Train::getTrainNumber(trainIndex);
                        transmitToCommandServer(Constants::MSG::RV);
                        return;
                    } else {
                        bwprintf(COM2, "Navigation Server - Merge direction selection failed!\n\r");
                    }
                    break;
                }
                case NODE_TYPE::NODE_ENTER:
                {
                    break;
                }
                case NODE_TYPE::NODE_EXIT:
                {
                    break;
                }
                case NODE_TYPE::NODE_NONE:
                {
                    break;
                }
                default:
                {
                    bwprintf(COM2, "Navigation Server - Invalid Node type %d encountered\n\r", currentLandmark->type);
                    break;
                }
            }
            currentLandmark = nextLandmark;
            currentLandmarkName = nextLandmarkName;
        }
    }
}

void NavigationServer::init() {
    // TODO: (spratap) add support for both tracks
    commandCourier = Create(7, navCSCourier);
    trainCourier = Create(5, navTrainCourier);
}

void navigationServer() {
    RegisterAs("NAV");
    ThinMessage rdymsg(Constants::MSG::RDY);
    ThinMessage errmsg(Constants::MSG::ERR);

    NavigationServer ns;
    ns.init();

    int tid;

    FOREVER {
        Receive(&tid, (char*)&ns.msg, 128);

        if (ns.mh->type == Constants::MSG::RDY) {
            if (tid == ns.commandCourier) {
                ns.commandCourierReady = true;
                ns.navigate();
            } else if (tid == ns.trainCourier) {
                ns.trainCourierReady = true;
            }
        } else if (ns.mh->type == Constants::MSG::RT) {
            Reply(tid, (char*)&rdymsg, rdymsg.size());
            bool pathExist = ns.findPath();
            if (pathExist) {

                ns.navigate(); // start nagivation (will start train)
                ns.initSensorPredictions(); // initialize train predictions
            }
        } else if (ns.mh->type == Constants::MSG::DT) {
            // TODO: Implement me
            // Once we have information it should be possible to change destination
        } else if (ns.mh->type == Constants::MSG::TP) {
            // Placeholder, super low priority to implement this
        } else if (ns.mh->type == Constants::MSG::SENSOR_ATTR) {
            ns.predictSensors();
            Reply(tid, (char*)&ns.spmsg, ns.spmsg.size());
            ns.spmsg.count = 0;

            if (ns.commandCourierReady) {
                ns.navigate();
            }
        } else {
            bwprintf(COM2, "Navigation Server - Received an unexpected message: %d", ns.mh->type);
        }
    }
}
