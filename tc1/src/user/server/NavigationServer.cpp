#include <string.h>

#include "Constants.hpp"
#include "Util.hpp"
#include "data-structures/RingBuffer.hpp"
#include "data-structures/Stack.hpp"
#include "io/bwio.hpp"
#include "io/StringFormat.hpp"
#include "user/courier/NavCommandCourier.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/DTMessage.hpp"
#include "user/message/RTMessage.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/SensorAttrMessage.hpp"
#include "user/message/SensorPredMessage.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/TPMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/server/NavigationServer.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "user/trains/Track.hpp"

#define FOREVER for(;;)

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
        }
    } while(!djikstraFinished(track, visitedNodes));
}


void NavigationServer::init() {
    // TODO: (spratap) add support for both tracks
    // track = Track('A');
    commandCourier = Create(7, navCommandCourier);
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
        if (track.trackNodes[i].reserved) {
            bwprintf(COM2, "%d res ", i);
        }
    }

    if (srcIndex == -1 || destIndex == -1) {
        bwprintf(COM2, "Navigation Server continue- Bad src <%s> [%d]  or dest <%s> [%d]\n\r", rtmsg->src, srcIndex, rtmsg->dest, destIndex);
        return;
    }

    djikstra(&track, srcIndex, destIndex, lastHop);

    // TODO: (spratap) Analyze if we need to turn off tracks here?
    int index = destIndex;
    do {
        if (index == -1) {
            bwprintf(COM2, "Navigation Server - No Path Exists from <%s> to <%s>\n\r", rtmsg->src, rtmsg->dest);
            paths[rtmsg->train].reset();
            sensorLists[rtmsg->train].reset();
            return;
        }
        paths[rtmsg->train].push(index);
        if (track.trackNodes[index].type == NODE_SENSOR) {
            int num = track.trackNodes[index].num;
            int b = num / 16 + 'A';
            int n = num % 16 + 1;
            sensorLists[rtmsg->train].push(Sensor(b,n));
        }
        index = lastHop[index];
    } while (index != srcIndex);
    paths[rtmsg->train].push(index);
    if (track.trackNodes[index].type == NODE_SENSOR) {
        int num = track.trackNodes[index].num;
        int b = num / 16 + 'A';
        int n = num % 16 + 1;
        sensorLists[rtmsg->train].push(Sensor(b,n));
    } else {
        bwprintf(COM2, "Navigation Server - Destination is not a Sensor <%s>\n\r", rtmsg->dest);
    }
}

void NavigationServer::attributeSensors() {
    //TODO:(spratap) pop stuff off from path too
    for (int i = 0; i < samsg->count; ++i) {
        if (sensorLists[samsg->sensorAttrs[i].train].peek() == samsg->sensorAttrs[i].sensor) {
            // Assert(paths[samsg->sensorAttrs[i].train].peek() == samsg->sensorAttrs[i].sensor);
            paths[samsg->sensorAttrs[i].train].pop();
            sensorLists[samsg->sensorAttrs[i].train].pop();
            spmsg.predictions[spmsg.count].nextSensor[0] = sensorLists[samsg->sensorAttrs[i].train].peek();
            spmsg.predictions[spmsg.count].nextSensor[1] = sensorLists[samsg->sensorAttrs[i].train].peekSecond();
            spmsg.predictions[spmsg.count].train = samsg->sensorAttrs[i].train;
            spmsg.count++;
        } else if (sensorLists[samsg->sensorAttrs[i].train].peekSecond() == samsg->sensorAttrs->sensor) {
            bwprintf(COM2, "Fucked");
            sensorLists[samsg->sensorAttrs[i].train].pop();
            sensorLists[samsg->sensorAttrs[i].train].pop();
            paths[samsg->sensorAttrs[i].train].pop();
            paths[samsg->sensorAttrs[i].train].pop();
            spmsg.predictions[spmsg.count].nextSensor[0] = sensorLists[samsg->sensorAttrs[i].train].peek();
            spmsg.predictions[spmsg.count].nextSensor[1] = sensorLists[samsg->sensorAttrs[i].train].peekSecond();
            spmsg.predictions[spmsg.count].train = samsg->sensorAttrs[i].train;
            spmsg.count++;
        } else { // no match found
            bwprintf(COM2, "Nav Server - Bad Sensor Attribution..");
        }
    }
}

void NavigationServer::navigate() {
    for (int train = 0; train < 5; ++train) {
        if (paths[train].empty()) {
            continue;
        } else {
            int top = paths[train].peek();
            if (track.trackNodes[top].type != NODE_TYPE::NODE_SENSOR) {
                paths[train].pop();
                Assert(paths[train].empty() == false);
                switch(track.trackNodes[top].type) {
                    case NODE_TYPE::NODE_BRANCH:
                    {
                        int nextNodeIndex = paths[train].peek();
                        const char* nextNodeName = track.trackNodes[nextNodeIndex].name;
                        const char* straightDirectionName = track.trackNodes[top].edges[DIR_STRAIGHT].destNode->name;
                        const char* curvedDirectionName = track.trackNodes[top].edges[DIR_CURVED].destNode->name;

                        if (!strcmp(nextNodeName, straightDirectionName)) {
                            // bwprintf(COM2, "Straight SW %s -> ", track.trackNodes[top].name);
                            swmsg.sw = track.trackNodes[top].edges[DIR_STRAIGHT].destNode->num;
                            swmsg.state = 'S';
                        } else if(!strcmp(nextNodeName, curvedDirectionName)) {
                            // bwprintf(COM2, "Curved SW %s -> ", track.trackNodes[top].name);
                            swmsg.sw = track.trackNodes[top].edges[DIR_CURVED].destNode->num;
                            swmsg.state = 'C';
                        } else {
                            bwprintf(COM2, "Navigation Client - Error Pathfinding!!\n\r");
                        }
                        Reply(commandCourier, (char*)&trmsg, trmsg.size());
                        commandCourierReady = false;
                        break;
                    }
                    case NODE_TYPE::NODE_MERGE:
                    {
                        int nextNodeIndex = paths[train].peek();
                        const char* nextNodeName = track.trackNodes[nextNodeIndex].name;
                        const char* aheadDirectionName = track.trackNodes[top].edges[DIR_AHEAD].destNode->name;
                        const char* reverseDirectionName = track.trackNodes[top].reverseNode->name;
                        if (!strcmp(nextNodeName, aheadDirectionName)) {
                            // bwprintf(COM2, "Straight %s -> ", track.trackNodes[top].name);
                        } else if(!strcmp(nextNodeName, reverseDirectionName)) {
                            // bwprintf(COM2, "Reverse %s -> ", track.trackNodes[top].name);
                            rvmsg.train = train;
                            rvmsg.speed = 14;
                            Reply(commandCourier, (char*)&rvmsg, rvmsg.size());
                            commandCourierReady = false;
                        } else {
                            // bwprintf(COM2, track.trackNodes[top].name);
                        }
                        break;
                    }
                    default: {
                        // bwprintf(COM2, "%s ->", track.trackNodes[top].name);
                        break;
                    }
                }
                // top = paths[samsg->sensorAttrs[i].train].pop();
            }
        }
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
            ns.commandCourierReady = true;
            ns.navigate();
        } else if (ns.mh->type == Constants::MSG::RT) {
            ns.findPath();
            Reply(tid, (char*)&rdymsg, rdymsg.size());

            // TODO: (spratap) start the train here
            // ns.trmsg.train = ns.rtmsg->train;
            // ns.trmsg.speed = 14;
            // ns.trmsg.headlights = true;
            // Reply(ns.commandCourier, (char*)&ns.trmsg, ns.trmsg.size());

        } else if (ns.mh->type == Constants::MSG::DT) {
            // bwprintf(COM2, "Nav - DT Rec");
            // Once we have information it should be possible to change destination
        } else if (ns.mh->type == Constants::MSG::TP) {
            // Placeholder, super low priority to implement this
        } else if (ns.mh->type == Constants::MSG::SENSOR_ATTR) {
            ns.attributeSensors();
            Reply(tid, (char*)&ns.spmsg, ns.spmsg.size());
            if (ns.commandCourierReady) {
                ns.navigate();
            }
            ns.spmsg.count = 0;
        }
    }
}