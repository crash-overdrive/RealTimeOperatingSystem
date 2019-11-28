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

Sensor convertToSensor(const Track* track, int index) {
    if (track->trackNodes[index].type != NODE_TYPE::NODE_SENSOR) {
        return Sensor(0,0);
    } else {
        int num = track->trackNodes[index].num;
        return Sensor(num / 16 + 'A',num % 16 + 1);
    }
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
            int indexAhead = track->trackNodes[srcIndex].edges[DIR_AHEAD].destNode - &track->trackNodes[0];
            if (!track->trackNodes[indexAhead].reserved) {
                dist[indexAhead] = track->trackNodes[srcIndex].edges[DIR_AHEAD].dist;
                lastHop[indexAhead] = srcIndex;
            } else {
                bwprintf(COM2, "Navigation Server - Initial Starting point straight is reserved\n\r");
            }

            int indexReverse = track->trackNodes[srcIndex].reverseNode - &track->trackNodes[0];
            if (!track->trackNodes[indexReverse].reserved) {
                dist[indexReverse] = 0;
                lastHop[indexReverse] = srcIndex;
            } else {
                bwprintf(COM2, "Navigation Server - Initial Starting point reverse is reserved\n\r");
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
                int indexAhead = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].destNode - &track->trackNodes[0];
                if (visitedNodes[indexAhead] == false && !track->trackNodes[indexAhead].reserved) {
                    int distToNeighbour = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].dist;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[indexAhead]) {
                        dist[indexAhead] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[indexAhead] = minVertexNotVisited;
                    }
                }
                int indexReverse = track->trackNodes[minVertexNotVisited].reverseNode - &track->trackNodes[0];
                if (visitedNodes[indexReverse] == false && !track->trackNodes[indexReverse].reserved) {
                    int distToNeighbour = 0;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[indexReverse]) {
                        dist[indexReverse] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[indexReverse] = minVertexNotVisited;
                    }
                }
                break;
            }

            case NODE_TYPE::NODE_BRANCH:
            {
                int indexStraight = track->trackNodes[minVertexNotVisited].edges[DIR_STRAIGHT].destNode - &track->trackNodes[0];
                if (visitedNodes[indexStraight] == false && !track->trackNodes[indexStraight].reserved) {
                    int distToNeighbour = track->trackNodes[minVertexNotVisited].edges[DIR_STRAIGHT].dist;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[indexStraight]) {
                        dist[indexStraight] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[indexStraight] = minVertexNotVisited;
                    }
                }
                int indexCurved = track->trackNodes[minVertexNotVisited].edges[DIR_CURVED].destNode - &track->trackNodes[0];
                if (visitedNodes[indexCurved] == false && !track->trackNodes[indexCurved].reserved) {
                    int distToNeighbour = track->trackNodes[minVertexNotVisited].edges[DIR_CURVED].dist;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[indexCurved]) {
                        dist[indexCurved] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[indexCurved] = minVertexNotVisited;
                    }
                }
                int indexReverse = track->trackNodes[minVertexNotVisited].reverseNode - &track->trackNodes[0];
                if (visitedNodes[indexReverse] == false && !track->trackNodes[indexReverse].reserved) {
                    int distToNeighbour = 0;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[indexReverse]) {
                        dist[indexReverse] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[indexReverse] = minVertexNotVisited;
                    }
                }
                break;
            }

            case NODE_TYPE::NODE_MERGE:
            {
                int indexAhead = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].destNode - &track->trackNodes[0];
                if (visitedNodes[indexAhead] == false && !track->trackNodes[indexAhead].reserved) {
                    int distToNeighbour = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].dist;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[indexAhead]) {
                        dist[indexAhead] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[indexAhead] = minVertexNotVisited;
                    }
                }
                int indexReverse = track->trackNodes[minVertexNotVisited].reverseNode - &track->trackNodes[0];
                if (visitedNodes[indexReverse] == false && !track->trackNodes[indexReverse].reserved) {
                    int distToNeighbour = 0;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[indexReverse]) {
                        dist[indexReverse] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[indexReverse] = minVertexNotVisited;
                    }
                }
                break;
            }

            case NODE_TYPE::NODE_ENTER:
            {
                int indexAhead = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].destNode - &track->trackNodes[0];
                if (visitedNodes[indexAhead] == false && !track->trackNodes[indexAhead].reserved) {
                    int distToNeighbour = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].dist;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[indexAhead]) {
                        dist[indexAhead] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[indexAhead] = minVertexNotVisited;
                    }
                }
                break;
            }

            case NODE_TYPE::NODE_EXIT:
            {
                int indexReverse = track->trackNodes[minVertexNotVisited].reverseNode - &track->trackNodes[0];
                if (visitedNodes[indexReverse] == false && !track->trackNodes[indexReverse].reserved) {
                    int distToNeighbour = 0;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[indexReverse]) {
                        dist[indexReverse] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[indexReverse] = minVertexNotVisited;
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

void NavigationServer::findPath() {
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
        // Note: enclosing in brackets ()[]{}<> is a debugging tactic, not intended for actual server messages/kernel panics!
        bwprintf(COM2, "Navigation Server - Bad src <%s> [%d]  or dest <%s> [%d]\n\r", rtmsg->src, srcIndex, rtmsg->dest, destIndex);
        return;
    }

    djikstra(&track, srcIndex, destIndex, lastHop);

    // TODO: (spratap) Analyze if we need to turn off tracks here?
    int index = destIndex;
    int trainIndex = Train::getTrainIndex(rtmsg->train);
    do {
        if (index == -1) {
            bwprintf(COM2, "Navigation Server - No Path Exists from %s to %s\n\r", rtmsg->src, rtmsg->dest); // TODO(sgaweda): This actually shouldn't never happen and if it can we have to find a way around it!
            paths[trainIndex].reset();
            sensorLists[trainIndex].reset();
            return;
        }
        paths[trainIndex].push(index);
        if (track.trackNodes[index].type == NODE_SENSOR) {
            int num = track.trackNodes[index].num;
            char b = (char)(num / 16 + 'A');
            char n = (char)(num % 16 + 1);
            sensorLists[trainIndex].push(Sensor(b,n));
        }
        // bwprintf(COM2, "%s <- ", track.trackNodes[index].name);
        index = lastHop[index];
    } while (index != srcIndex);
    paths[trainIndex].push(index);
    // bwprintf(COM2, "%s\n\r", track.trackNodes[index].name);
    if (track.trackNodes[index].type == NODE_SENSOR) {
        int num = track.trackNodes[index].num;
        int b = num / 16 + 'A';
        int n = num % 16 + 1;
        sensorLists[trainIndex].push(Sensor(b,n));
    } else {
        bwprintf(COM2, "Navigation Server - Destination %s is not a sensor\n\r", rtmsg->dest); // TODO(sgaweda): If destinations must be sensors then we need to restrict the input too so we can save computation!
    }
}

void NavigationServer::predictSensors() {
    // check what type of message wants us to predict
    if (mh->type == Constants::SENSOR_ATTR) {
        for (int i = 0; i < samsg->count; ++i) {
            char train = samsg->sensorAttrs[i].train;
            int index = Train::getTrainIndex(train);
            if (sensorLists[index].peek() == samsg->sensorAttrs[i].sensor) {
                ASSERT((samsg->sensorAttrs[i].sensor == convertToSensor(&track, paths[index].peek())));
                int temp = paths[index].pop();
                // bwprintf(COM2, "Sensor %s tripped\n\r", track.trackNodes[temp].name);
                sensorLists[index].pop();
                spmsg.predictions[spmsg.count].nextSensor[0] = sensorLists[index].peek();
                spmsg.predictions[spmsg.count].nextSensor[1] = sensorLists[index].peekSecond();
                spmsg.predictions[spmsg.count].train = train;
                spmsg.count++;
            } else if (sensorLists[index].peekSecond() == samsg->sensorAttrs->sensor) {
                // TODO: handle recovery logic if needed
                paths[index].pop();
                paths[index].pop();
                sensorLists[index].pop();
                sensorLists[index].pop();
                spmsg.predictions[spmsg.count].nextSensor[0] = sensorLists[index].peek();
                spmsg.predictions[spmsg.count].nextSensor[1] = sensorLists[index].peekSecond();
                spmsg.predictions[spmsg.count].train = train;
                spmsg.count++;
            } else {
                bwprintf(COM2, "Nav Server - Bad Sensor Attribution.."); // NOTE(sgaweda): This should NEVER happen, if a train attributes a sensor incorrectly, it's because the Navigation Server told it to!
            }



            // TODO: refactor this logic out of here
            if (sensorLists[index].peek().bank == 0) {
                // STOP TRAIN
                trmsg.speed = 0;
                trmsg.train = train;
                Reply(commandCourier, (char*)&trmsg, trmsg.size());
                commandCourierReady = false;
                // bwprintf(COM2, "CC not ready\n\r");
            } else if (sensorLists[index].peekSecond().bank == 0) {
                // SLOW TRAIN
                trmsg.speed = 8;
                trmsg.train = train;
                Reply(commandCourier, (char*)&trmsg, trmsg.size());
                commandCourierReady = false;
                // bwprintf(COM2, "CC not ready\n\r");
            }



        }
    } else if (mh->type == Constants::RT) {
        int index = Train::getTrainIndex(rtmsg->train);
        spmsg.predictions[spmsg.count].nextSensor[0] = sensorLists[index].peek();
        spmsg.predictions[spmsg.count].nextSensor[1] = sensorLists[index].peekSecond();
        spmsg.predictions[spmsg.count].train = rtmsg->train;
        spmsg.count++;
    }
}

void NavigationServer::navigate() {
    ASSERT(commandCourierReady == true);
    // bwprintf(COM2, "Navigate Called!!\n\r");
    for (int train = 0; train < 5; ++train) {
        // Do nothing if there's no path
        if (paths[train].empty()) {
            continue;
        }

        const TrackNode* currentLandmark = &track.trackNodes[paths[train].peek()];
        const char* currentLandmarkName = currentLandmark->name;

        if (currentLandmark->type == NODE_TYPE::NODE_SENSOR) {
            //TODO: implement me for reversing
            // bwprintf(COM2, "Train %d Waiting for Sensor %s\n\r", train, currentLandmarkName);
            continue;
        }

        while(currentLandmark->type != NODE_TYPE::NODE_SENSOR) {
            int temp = paths[train].pop();
            // bwprintf(COM2, "Popped %s\n\r", track.trackNodes[temp].name);
            ASSERT(paths[train].empty() == false);
            const TrackNode* nextLandmark =  &track.trackNodes[paths[train].peek()];
            const char* nextLandmarkName = nextLandmark->name;

            switch (currentLandmark->type) {
                case NODE_TYPE::NODE_BRANCH:
                {
                    const char* straightDirectionLandmarkName = currentLandmark->edges[DIR_STRAIGHT].destNode->name;
                    const char* curvedDirectionLandmarkName = currentLandmark->edges[DIR_CURVED].destNode->name;

                    if (!strcmp(nextLandmarkName, straightDirectionLandmarkName)) {
                        swmsg.sw = currentLandmark->num;
                        swmsg.state = 'S';
                        int result = Reply(commandCourier, (char*)&swmsg, swmsg.size());
                        commandCourierReady = false;
                        // bwprintf(COM2, "CC not ready\n\r");
                        // bwprintf(COM2, "[%d %d %c swmsg]\n\r", result, swmsg.sw, swmsg.state);
                        // bwprintf(COM2, "[Straight SW %s]\n\r", currentLandmarkName);
                        return;
                    } else if (!strcmp(nextLandmarkName, curvedDirectionLandmarkName)) {
                        swmsg.sw = currentLandmark->num;
                        swmsg.state = 'C';
                        int result = Reply(commandCourier, (char*)&swmsg, swmsg.size());
                        commandCourierReady = false;
                        // bwprintf(COM2, "CC not ready\n\r");
                        // bwprintf(COM2, "[%d %d %c swmsg]\n\r", result, swmsg.sw, swmsg.state);
                        // bwprintf(COM2, "[Curved SW %s]\n\r", currentLandmarkName);
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
                        // bwprintf(COM2, "[Ahead %s]\n\r", aheadDirectionLandmarkName);
                    } else if(!strcmp(nextLandmarkName, reverseDirectionLandmarkName)) {
                        rvmsg.train = Train::getTrainNumber(train);
                        Reply(commandCourier, (char*)&rvmsg, rvmsg.size());
                        commandCourierReady = false;
                        // bwprintf(COM2, "CC not ready\n\r");
                        // bwprintf(COM2, "[Reverse %s]\n\r", reverseDirectionLandmarkName);
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
        // bwprintf(COM2, "Out of while loop %s\n\r", currentLandmarkName);
    }
    // bwprintf(COM2, "Navigate Returning\n\r");
}

void NavigationServer::init() {
    // TODO: (spratap) add support for both tracks
    // track = Track('A');
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
                // bwprintf(COM2, "CC Ready\n\r");
                ns.commandCourierReady = true;
                ns.navigate();
            } else if (tid == ns.trainCourier) {
                ns.trainCourierReady = true;
            }
        } else if (ns.mh->type == Constants::MSG::RT) {
            ns.findPath();

            ns.predictSensors();
            ASSERT(ns.trainCourierReady); // This should always be true because it's impossible for lower priority command server to run before the train server gets back to us.
            Reply(ns.trainCourier, (char*)&ns.spmsg, ns.spmsg.size());
            ns.trainCourierReady = false;

            // TODO(sgaweda): Can this courier be blocked when this happens? I suspect it's possible so we should probably check
            // Maybe refactor this to a start train function?
            // TODO(sgaweda): train control will be responsible for determining if this train speed change needs to occur. We can assume train is stopped here.
            ns.trmsg.train = ns.rtmsg->train;
            ns.trmsg.speed = 14;
            Reply(ns.commandCourier, (char*)&ns.trmsg, ns.trmsg.size());
            ns.commandCourierReady = false;
            // bwprintf(COM2, "CC not ready\n\r");

            // We always reply to the requesting task last. Yes it blocks for longer but we're not ready to process another parsed command anyway. Plus no one types that fast!
            Reply(tid, (char*)&rdymsg, rdymsg.size());
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
        }
    }
}
