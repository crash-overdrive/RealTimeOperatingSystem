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
                dist[trackIndexReverse] = reverseClearance;
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
                    int distToNeighbour = reverseClearance;
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
                    int distToNeighbour = reverseClearance;
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
                    int distToNeighbour = reverseClearance;
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

void NavigationServer::init() {
    // TODO: (spratap) add support for both tracks
    commandCourier = Create(7, navCSCourier);
    trainCourier = Create(5, navTrainCourier);
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

void NavigationServer::queueCommand(TrackCommand trackCommand) {
    cmdqueue.push(trackCommand);
    if (commandCourierReady) {
        issueCommand();
    }
}

void NavigationServer::issueCommand() {
    ASSERT(commandCourierReady);
    if (!cmdqueue.empty()) {
        commandCourierReady = false;
        TrackCommand trackCommand = cmdqueue.pop();
        switch(trackCommand.type) {
            case COMMANDS::SWITCH:
            {
                SWMessage msg;
                msg.sw = trackCommand.tr_sw_rv.sw.number;
                msg.state = trackCommand.tr_sw_rv.sw.orientation;
                Reply(commandCourier, (char*)&msg, msg.size());
                break;
            }
            case COMMANDS::TRAIN:
            {
                TRMessage msg;
                msg.train = trackCommand.tr_sw_rv.tr.train;
                msg.speed = trackCommand.tr_sw_rv.tr.speed;
                trainSpeed[Train::getTrainIndex(trackCommand.tr_sw_rv.tr.train)] = trackCommand.tr_sw_rv.tr.speed;
                Reply(commandCourier, (char*)&msg, msg.size());
                break;
            }
            case COMMANDS::REVERSE:
            {
                RVMessage msg;
                msg.train = trackCommand.tr_sw_rv.rv.train;
                Reply(commandCourier, (char*)&msg, msg.size());
                break;
            }
            default:
            {
                bwprintf(COM2, "Navigation Server - illegal command tried to be issued: %d", trackCommand.type);
                break;
            }
        }
    }
}

void NavigationServer::initSensorPredictions() {
    int trainIndex = Train::getTrainIndex(rtmsg->train);
    spmsg.predictions[spmsg.count].nextSensor[0] = Track::getSensor(sensorLists[trainIndex].peek());
    spmsg.predictions[spmsg.count].nextSensor[1] = Track::getSensor(sensorLists[trainIndex].peekSecond());
    spmsg.predictions[spmsg.count].nextSensorDistance[0] = sensorDistanceLists[trainIndex].peek();
    spmsg.predictions[spmsg.count].nextSensorDistance[1] = sensorDistanceLists[trainIndex].peekSecond();
    spmsg.predictions[spmsg.count].train = rtmsg->train;
    spmsg.count++;
    transmitToTrainServer(Constants::MSG::SENSOR_PRED);
    spmsg.count = 0;
}

void NavigationServer::predictSensors() {
    for (int i = 0; i < samsg->count; ++i) {
        // bwprintf(COM2, "Sensor %c%d tripped\n\r", samsg->sensorAttrs[i].sensor.bank, samsg->sensorAttrs[i].sensor.number);
        char train = samsg->sensorAttrs[i].train;
        int trainIndex = Train::getTrainIndex(train);
        if (sensorLists[trainIndex].peek() == Track::getIndex(samsg->sensorAttrs[i].sensor)) {
            int sensorHitIndex = sensorLists[trainIndex].pop();
            sensorDistanceLists[trainIndex].pop();

            while(sensorHitIndex != paths[trainIndex].peek()) {
                unreserveTrack(paths[trainIndex].pop());
                landmarkDistanceLists[trainIndex].pop();
            }
            if (sensorLists[trainIndex].empty()) {
                unreserveTrack(sensorHitIndex);
            }
        } else if (sensorLists[trainIndex].peekSecond() == Track::getIndex(samsg->sensorAttrs->sensor)) {
            // TODO: handle recovery logic if needed
            sensorLists[trainIndex].pop();
            int sensorHitIndex = sensorLists[trainIndex].pop();
            sensorDistanceLists[trainIndex].pop();
            sensorDistanceLists[trainIndex].pop();

            while(sensorHitIndex != paths[trainIndex].peek()) {
                unreserveTrack(paths[trainIndex].pop());
                landmarkDistanceLists[trainIndex].pop();
            }
            if (sensorLists[trainIndex].empty()) {
                unreserveTrack(sensorHitIndex);
            }
        } else {
            bwprintf(COM2, "Nav Server - Bad Sensor Attribution.."); // NOTE(sgaweda): This should NEVER happen, if a train attributes a sensor incorrectly, it's because the Navigation Server told it to!
            return;
        }

        spmsg.predictions[spmsg.count].nextSensor[0] = Track::getSensor(sensorLists[trainIndex].peek());
        spmsg.predictions[spmsg.count].nextSensor[1] = Track::getSensor(sensorLists[trainIndex].peekSecond());
        spmsg.predictions[spmsg.count].nextSensorDistance[0] = sensorDistanceLists[trainIndex].peek();
        spmsg.predictions[spmsg.count].nextSensorDistance[1] = sensorDistanceLists[trainIndex].peekSecond();
        spmsg.predictions[spmsg.count].train = train;
        spmsg.count++;
    }
}

void NavigationServer::reserveTrack() {
    while(!reservationsList.empty()) {
        int trackIndex = reservationsList.pop();
        track.trackNodes[trackIndex].reserved = true;
        track.trackNodes[trackIndex].reverseNode->reserved = true;
        bwprintf(COM2, "Reserved %s %s\n\r", track.trackNodes[trackIndex].name, track.trackNodes[trackIndex].reverseNode->name);
    }
}

void NavigationServer::unreserveTrack(int trackIndex) {
    track.trackNodes[trackIndex].reserved = false;
    track.trackNodes[trackIndex].reverseNode->reserved = false;
    bwprintf(COM2, "Unreserved %s %s\n\r", track.trackNodes[trackIndex].name, track.trackNodes[trackIndex].reverseNode->name);
}

bool NavigationServer::findPath() {
    int trainIndex = Train::getTrainIndex(rtmsg->train);
    ASSERT(paths[trainIndex].size() == 0);
    ASSERT(reverseList[trainIndex].size() == 0);
    ASSERT(branchList[trainIndex].size() == 0);
    ASSERT(sensorLists[trainIndex].size() == 0);
    ASSERT(sensorDistanceLists[trainIndex].size() == 0);
    ASSERT(landmarkDistanceLists[trainIndex].size() == 0);
    ASSERT(reservationsList.size() == 0);

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
    int sensorDistance = 0;

    do {
        nextTrackIndex = currentTrackIndex;
        currentTrackIndex = currentTrackIndex == -1 ? destIndex : lastHop[currentTrackIndex];

        // error checking
        if (currentTrackIndex == -1) {
            bwprintf(COM2, "Navigation Server - No Path Exists from %s to %s\n\r", rtmsg->src, rtmsg->dest); // TODO(sgaweda): This actually shouldn't never happen and if it can we have to find a way around it!
            paths[trainIndex].reset();
            reverseList[trainIndex].reset();
            branchList[trainIndex].reset();
            sensorLists[trainIndex].reset();
            sensorDistanceLists[trainIndex].reset();
            landmarkDistanceLists[trainIndex].reset();
            reservationsList.reset();
            return false;
        }

        // push to paths List
        paths[trainIndex].push(currentTrackIndex);

        //push to reservationList
        reservationsList.push(currentTrackIndex);

        switch (track.trackNodes[currentTrackIndex].type) {
            case NODE_TYPE::NODE_SENSOR:
            {
                // push to sensor List
                sensorLists[trainIndex].push(currentTrackIndex);

                int trackIndexAhead = track.trackNodes[currentTrackIndex].edges[DIR_AHEAD].destNode - &track.trackNodes[0];
                int trackIndexReverse = track.trackNodes[currentTrackIndex].reverseNode - &track.trackNodes[0];
                if (nextTrackIndex == -1) {
                    sensorDistance = 0;
                    landmarkDistanceLists[trainIndex].push(sensorDistance);
                } else if (trackIndexAhead == nextTrackIndex) {
                    sensorDistance += track.trackNodes[currentTrackIndex].edges[DIR_AHEAD].dist;
                    // push to landmarkDistance list
                    landmarkDistanceLists[trainIndex].push(track.trackNodes[currentTrackIndex].edges[DIR_AHEAD].dist);
                } else if (trackIndexReverse == nextTrackIndex) {
                    // push to reverse List
                    reverseList[trainIndex].push(currentTrackIndex);

                    sensorDistance += reverseClearance;
                    // push to landmarkDistance list
                    landmarkDistanceLists[trainIndex].push(reverseClearance);
                } else {
                    bwprintf(COM2, "Navigation Server - Error initializing Sensor distance list, node sensor\n\r");
                }

                // push to sensorDistances List
                sensorDistanceLists[trainIndex].push(sensorDistance);
                sensorDistance = 0;
                break;
            }
            case NODE_TYPE::NODE_BRANCH:
            {
                SW msg;
                msg.number = currentTrackIndex;

                int trackIndexStraight = track.trackNodes[currentTrackIndex].edges[DIR_STRAIGHT].destNode - &track.trackNodes[0];
                int trackIndexCurved = track.trackNodes[currentTrackIndex].edges[DIR_CURVED].destNode - &track.trackNodes[0];
                if (trackIndexStraight == nextTrackIndex) {
                    sensorDistance += track.trackNodes[currentTrackIndex].edges[DIR_STRAIGHT].dist;
                    // push to branch List
                    msg.orientation = 'S';
                    branchList[trainIndex].push(msg);
                    // push to landmarkDistance list
                    landmarkDistanceLists[trainIndex].push(track.trackNodes[currentTrackIndex].edges[DIR_STRAIGHT].dist);
                } else if (trackIndexCurved == nextTrackIndex) {
                    sensorDistance += track.trackNodes[currentTrackIndex].edges[DIR_CURVED].dist;
                     // push to branch List
                    msg.orientation = 'C';
                    branchList[trainIndex].push(msg);
                    // push to landmarkDistance list
                    landmarkDistanceLists[trainIndex].push(track.trackNodes[currentTrackIndex].edges[DIR_CURVED].dist);
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
                    sensorDistance += track.trackNodes[currentTrackIndex].edges[DIR_AHEAD].dist;
                    // push to landmarkDistance list
                    landmarkDistanceLists[trainIndex].push(track.trackNodes[currentTrackIndex].edges[DIR_AHEAD].dist);
                } else if (trackIndexReverse == nextTrackIndex) {
                    // push to reverse List
                    reverseList[trainIndex].push(currentTrackIndex);

                    sensorDistance += reverseClearance;
                    // push to landmarkDistance list
                    landmarkDistanceLists[trainIndex].push(reverseClearance);
                } else {
                    bwprintf(COM2, "Navigation Server - Error initializing Sensor distance list, node merge\n\r");
                }
                break;
            }
            case NODE_TYPE::NODE_ENTER:
            {
                int trackIndexAhead = track.trackNodes[currentTrackIndex].edges[DIR_AHEAD].destNode - &track.trackNodes[0];
                if (trackIndexAhead == nextTrackIndex) {
                    sensorDistance += track.trackNodes[currentTrackIndex].edges[DIR_AHEAD].dist;
                    // push to landmarkDistance list
                    landmarkDistanceLists[trainIndex].push(track.trackNodes[currentTrackIndex].edges[DIR_AHEAD].dist);
                } else {
                    bwprintf(COM2, "Navigation Server - Error initializing Sensor distance list, node enter\n\r");
                }
                break;
            }
            case NODE_TYPE::NODE_EXIT:
            {
                int trackIndexReverse = track.trackNodes[currentTrackIndex].reverseNode - &track.trackNodes[0];
                if (trackIndexReverse == nextTrackIndex) {
                    // push to reverse List
                    reverseList[trainIndex].push(currentTrackIndex);

                    sensorDistance += reverseClearance;
                    // push to landmarkDistance list
                    landmarkDistanceLists[trainIndex].push(reverseClearance);
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
    } while(currentTrackIndex != srcIndex);
    ASSERT(sensorLists[trainIndex].size() == sensorDistanceLists[trainIndex].size());
    ASSERT(landmarkDistanceLists[trainIndex].size() == paths[trainIndex].size());

    // reserve Track
    reserveTrack();
    stopCommandSent[trainIndex] = false;

    // bwprintf(COM2, "Landmark - Distance\n\r");
    // while(!paths[trainIndex].empty()) {
    //     bwprintf(COM2, "%s %d\n\r", track.trackNodes[paths[trainIndex].pop()].name, landmarkDistanceLists[trainIndex].pop());
    // }
    // bwprintf(COM2, "Sensor - Sensor Distance List\n\r");
    // while(!sensorLists[trainIndex].empty()) {
    //     bwprintf(COM2, "%s %d\n\r", track.trackNodes[sensorLists[trainIndex].pop()].name, sensorDistanceLists[trainIndex].pop());
    // }
    // bwprintf(COM2, "Reverse List\n\r");
    // while(!reverseList[trainIndex].empty()) {
    //     bwprintf(COM2, "%s\n\r", track.trackNodes[reverseList[trainIndex].pop()].name);
    // }
    // bwprintf(COM2, "Branches\n\r");
    // while(!branchList[trainIndex].empty()) {
    //     bwprintf(COM2, "%s\n\r", track.trackNodes[branchList[trainIndex].pop()].name);
    // }
    return true;
}

int NavigationServer::journeyLeft(int trainIndex, int offset) {

    DataStructures::Stack<int, TRACK_MAX> tempPath;
    DataStructures::Stack<int, 40> tempDistanceList;

    int distance = offset;

    while (!paths[trainIndex].empty()) {
        int trackIndex = paths[trainIndex].pop();
        int landMarkDistance = landmarkDistanceLists[trainIndex].pop();
        tempPath.push(trackIndex);
        tempDistanceList.push(landMarkDistance);
        distance = distance + landMarkDistance;

    }
    while(!tempPath.empty()) {
        paths[trainIndex].push(tempPath.pop());
        landmarkDistanceLists[trainIndex].push(tempDistanceList.pop());
    }
    return distance ;
}

void NavigationServer::evaluate(int trainIndex) {
    DataStructures::Stack<int, 40> tempPath;
    DataStructures::Stack<int, 40> tempDistanceList;

    Location location = lastKnownLocations[trainIndex];
    if (paths[trainIndex].peek() != location.landmark) {
        bwprintf(COM2, "Mismatch, expect %s got %s\n\r", track.trackNodes[paths[trainIndex].peek()].name, track.trackNodes[location.landmark].name);
    }
    ASSERT(paths[trainIndex].peek() == location.landmark);
    int distance = 0;

    while (distance < location.offset && !paths[trainIndex].empty()) {
        int trackIndex = paths[trainIndex].pop();
        // bwprintf(COM2, "Ignored %s as its already been evaluated\n\r", track.trackNodes[trackIndex].name);
        int landMarkDistance = landmarkDistanceLists[trainIndex].pop();
        tempPath.push(trackIndex);
        tempDistanceList.push(landMarkDistance);
        distance = distance + landMarkDistance;
    }
    if (paths[trainIndex].empty()) {
        // bwprintf(COM2, "Finished evaluating\n\r");
        ASSERT(reverseList[trainIndex].empty());
        ASSERT(branchList[trainIndex].empty());
    } else {
        distance = distance - location.offset;
        int distanceLeft = journeyLeft(trainIndex, distance);
        while (distance < 300 && !paths[trainIndex].empty()) {
            int trackIndex = paths[trainIndex].pop();
            int landMarkDistance = landmarkDistanceLists[trainIndex].pop();
            tempPath.push(trackIndex);
            tempDistanceList.push(landMarkDistance);
            distance = distance + landMarkDistance;

            if(branchList[trainIndex].peek().number == trackIndex) {
                bwprintf(COM2, "Found branch %s at %s %d\n\r", track.trackNodes[trackIndex].name, track.trackNodes[location.landmark].name, location.offset);
                SW sw = branchList[trainIndex].pop();
                TrackCommand trackCommand;
                trackCommand.type = COMMANDS::SWITCH;
                trackCommand.tr_sw_rv.sw.number = track.trackNodes[sw.number].num;
                trackCommand.tr_sw_rv.sw.orientation = sw.orientation;
                queueCommand(trackCommand);
            } else if (reverseList[trainIndex].peek() == trackIndex) {
                bwprintf(COM2, "Found reverse %s at %s %d\n\r", track.trackNodes[trackIndex].name, track.trackNodes[location.landmark].name, location.offset);
                reverseList[trainIndex].pop();
                TrackCommand trackCommand;
                trackCommand.type = COMMANDS::REVERSE;
                trackCommand.tr_sw_rv.rv.train = Train::getTrainNumber(trainIndex);
                queueCommand(trackCommand);
            }
        }

        if (distanceLeft < Train::stoppingDistances[trainIndex][trainSpeed[trainIndex]] / 1000 && !stopCommandSent[trainIndex]) { // && reverseList[trainIndex].empty()) {
            bwprintf(COM2, "Stop Loc: %s %d, dist left: %d, Stop dist: %d", track.trackNodes[location.landmark].name, location.offset, distanceLeft, Train::stoppingDistances[trainIndex][trainSpeed[trainIndex]]);
            stopCommandSent[trainIndex] = true;
            bwprintf(COM2, "Stop sent\n\r");
            // STOP TRAIN
            TrackCommand trackCommand;
            trackCommand.type = COMMANDS::TRAIN;
            trackCommand.tr_sw_rv.tr.train = Train::getTrainNumber(trainIndex);
            trackCommand.tr_sw_rv.tr.speed = 0;
            queueCommand(trackCommand);
        }
    }

    while (!tempPath.empty()) {
        paths[trainIndex].push(tempPath.pop());
        landmarkDistanceLists[trainIndex].push(tempDistanceList.pop());
    }
}

void NavigationServer::navigate() {
    for (int locationInfoIndex = 0; locationInfoIndex < locmsg->count; ++ locationInfoIndex) {
        int trainIndex = Train::getTrainIndex(locmsg->locationInfo[locationInfoIndex].train);

        //first update the lastKnownLocation and then evaluate the train
        Location* newLocation = &(locmsg->locationInfo[locationInfoIndex].location);
        lastKnownLocations[trainIndex] = Location(newLocation->landmark, newLocation->offset / 1000);

        // bwprintf(COM2, "Got L:%s %d, expect: %s", track.trackNodes[newLocation->landmark].name, newLocation->offset, track.trackNodes[paths[trainIndex].peek()].name);

        evaluate(trainIndex);
    }
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
                // bwprintf(COM2, "CC Ready\n\r");
                ns.commandCourierReady = true;
                ns.issueCommand();
            } else if (tid == ns.trainCourier) {
                ns.trainCourierReady = true;
            }

        } else if (ns.mh->type == Constants::MSG::RT) {

            Reply(tid, (char*)&rdymsg, rdymsg.size());
            bool pathExist = ns.findPath();
            if (pathExist) {
                ns.initSensorPredictions(); // initialize train predictions
                // TODO: take this out of here
                TrackCommand trackCommand;
                trackCommand.type = COMMANDS::TRAIN;
                trackCommand.tr_sw_rv.tr.train = ns.rtmsg->train;
                trackCommand.tr_sw_rv.tr.speed = 9;
                ns.queueCommand(trackCommand); //start the train
            }

        } else if (ns.mh->type == Constants::MSG::SENSOR_ATTR) {

            ns.predictSensors();
            Reply(tid, (char*)&ns.spmsg, ns.spmsg.size());
            ns.spmsg.count = 0;

        } else if (ns.mh->type == Constants::MSG::LOCATION) {

            Reply(tid, (char*)&rdymsg, rdymsg.size());
            ns.navigate();

        } else if (ns.mh->type == Constants::MSG::DT) {
            // TODO: Implement me
            // Once we have information it should be possible to change destination
        } else if (ns.mh->type == Constants::MSG::TP) {
            // Placeholder, super low priority to implement this
        } else {
            bwprintf(COM2, "Navigation Server - Received an unexpected message: %d", ns.mh->type);
        }
    }
}
