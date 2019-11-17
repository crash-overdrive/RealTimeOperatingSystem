#include <string.h>

#include "data-structures/Stack.hpp"
#include "user/client/NavigationClient.hpp"
#include "user/trains/Track.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "io/bwio.hpp"
#include "io/StringFormat.hpp"

int areAllValuesSame(bool arr[], int length, bool value) {
    for (int i = 0; i < length; ++i) {
        if (arr[i] != value) {
            return false;
        }
    }
    return true;
}

int minIndex(int arr[], bool valid[], int length) {
    int i = 0;
    while (valid[i] == false && i < length) {
        ++i;
    }
    if (i >= length) {
        return -1;
    }

    int minIndex = i;
    int minValue = arr[i];

    ++i;

    for (; i < length; ++i) {
        if (valid[i] == true && arr[i] < minValue) {
            minValue = arr[i];
            minIndex = i;
        }
    }
    return minIndex;
}

int minIndex(int arr[], int length) {
    int minIndex = 0;
    int minValue = arr[0];
    for (int i = 1; i < length; ++i) {
        if (arr[i] < minValue) {
            minValue = arr[i];
            minIndex = i;
        }
    }
    return minIndex;
}

void djikstra(Track* track, const char* src, const char* dest) {
    int srcIndex;
    int destIndex;

    int dist[track->noOfNodes];
    int lastHop[track->noOfNodes];
    bool notVisitedNodes[track->noOfNodes];
    DataStructures::Stack<int, TRACK_MAX> path;

    for (int i = 0; i < track->noOfNodes; ++i) {
        dist[i] = __INT_MAX__;
        lastHop[i] = -1;
        notVisitedNodes[i] = true;

        if (!strcmp(track->trackNodes[i].name, src)) {
            srcIndex = i;
        } else if(!strcmp(track->trackNodes[i].name, dest)) {
            destIndex = i;
        }
    }

    dist[srcIndex] = 0;
    notVisitedNodes[srcIndex] = false;

    switch(track->trackNodes[srcIndex].type) {
        case NODE_TYPE::NODE_SENSOR:
        {
            int indexAhead = track->trackNodes[srcIndex].edges[DIR_AHEAD].destNode - &track->trackNodes[0];
            dist[indexAhead] = track->trackNodes[srcIndex].edges[DIR_AHEAD].dist;
            lastHop[indexAhead] = srcIndex;
            break;
        }
        case NODE_TYPE::NODE_BRANCH:
        {
            int indexStraight = track->trackNodes[srcIndex].edges[DIR_STRAIGHT].destNode - &track->trackNodes[0];
            int indexCurved = track->trackNodes[srcIndex].edges[DIR_CURVED].destNode - &track->trackNodes[0];
            dist[indexStraight] = track->trackNodes[srcIndex].edges[DIR_STRAIGHT].dist;
            lastHop[indexStraight] = srcIndex;
            dist[indexCurved] = track->trackNodes[srcIndex].edges[DIR_CURVED].dist;
            lastHop[indexCurved] = srcIndex;
            break;
        }
        case NODE_TYPE::NODE_MERGE:
        {
            int indexAhead = track->trackNodes[srcIndex].edges[DIR_AHEAD].destNode - &track->trackNodes[0];
            dist[indexAhead] = track->trackNodes[srcIndex].edges[DIR_AHEAD].dist;
            lastHop[indexAhead] = srcIndex;
            break;
        }
        case NODE_TYPE::NODE_ENTER:
        {
            int indexAhead = track->trackNodes[srcIndex].edges[DIR_AHEAD].destNode - &track->trackNodes[0];
            dist[indexAhead] = track->trackNodes[srcIndex].edges[DIR_AHEAD].dist;
            lastHop[indexAhead] = srcIndex;
            break;
        }
        case NODE_TYPE::NODE_EXIT:
        default:
        {
            bwprintf(COM2, "Invalid start state!!");
            break;
        }
    }

    do {
        int minVertexNotVisited = minIndex(dist, notVisitedNodes, track->noOfNodes);
        if (minVertexNotVisited == -1) {
            bwprintf(COM2, "PANIC!!!");
        }
        notVisitedNodes[minVertexNotVisited] = false;

        switch(track->trackNodes[minVertexNotVisited].type) {
            case NODE_TYPE::NODE_SENSOR:
            {
                int indexAhead = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].destNode - &track->trackNodes[0];
                if (notVisitedNodes[indexAhead] == true) {
                    int distToNeighbour = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].dist;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[indexAhead]) {
                        dist[indexAhead] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[indexAhead] = minVertexNotVisited;
                    }
                }
                break;
            }
            case NODE_TYPE::NODE_BRANCH:
            {
                int indexStraight = track->trackNodes[minVertexNotVisited].edges[DIR_STRAIGHT].destNode - &track->trackNodes[0];
                if (notVisitedNodes[indexStraight] == true) {
                    int distToNeighbour = track->trackNodes[minVertexNotVisited].edges[DIR_STRAIGHT].dist;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[indexStraight]) {
                        dist[indexStraight] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[indexStraight] = minVertexNotVisited;
                    }
                }
                int indexCurved = track->trackNodes[minVertexNotVisited].edges[DIR_CURVED].destNode - &track->trackNodes[0];
                if (notVisitedNodes[indexCurved] == true) {
                    int distToNeighbour = track->trackNodes[minVertexNotVisited].edges[DIR_CURVED].dist;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[indexCurved]) {
                        dist[indexCurved] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[indexCurved] = minVertexNotVisited;
                    }
                }
                break;
            }

            case NODE_TYPE::NODE_MERGE:
            {
                int indexAhead = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].destNode - &track->trackNodes[0];
                if (notVisitedNodes[indexAhead] == true) {
                    int distToNeighbour = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].dist;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[indexAhead]) {
                        dist[indexAhead] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[indexAhead] = minVertexNotVisited;
                    }
                }
                break;
            }

            case NODE_TYPE::NODE_ENTER:
            {
                int indexAhead = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].destNode - &track->trackNodes[0];
                if (notVisitedNodes[indexAhead] == true) {
                    int distToNeighbour = track->trackNodes[minVertexNotVisited].edges[DIR_AHEAD].dist;
                    if (dist[minVertexNotVisited] + distToNeighbour < dist[indexAhead]) {
                        dist[indexAhead] = dist[minVertexNotVisited] + distToNeighbour;
                        lastHop[indexAhead] = minVertexNotVisited;
                    }
                }
                break;
            }

            case NODE_TYPE::NODE_EXIT:
                break;
        }
    } while(areAllValuesSame(notVisitedNodes, track->noOfNodes, false) == false);

    int index = destIndex;
    path.push(index);
    while (index != srcIndex) {
        index = lastHop[index];
        path.push(index);
    }

    while(!path.empty()) {
        int i = path.pop();
        switch(track->trackNodes[i].type) {
            case NODE_TYPE::NODE_BRANCH:
            {
                const char* nextNodeName = track->trackNodes[path.peek()].name;
                const char* straightDirectionName = track->trackNodes[i].edges[DIR_STRAIGHT].destNode->name;
                const char* curvedDirectionName = track->trackNodes[i].edges[DIR_CURVED].destNode->name;

                char msg[50];
                char reply[10];
                if (!strcmp(nextNodeName, straightDirectionName)) {
                    // int msglen = format(msg, "sw %d S", track->trackNodes[i].edges[DIR_STRAIGHT].destNode->num) + 1;
                    // Send(TRAIN_SERVER, msg, msglen, reply, 10);
                    bwprintf(COM2, "S %s ->", track->trackNodes[i].name);
                } else if(!strcmp(nextNodeName, curvedDirectionName)) {
                    // int msglen = format(msg, "sw %d C", track->trackNodes[i].edges[DIR_CURVED].destNode->num) + 1;
                    // Send(TRAIN_SERVER, msg, msglen, reply, 10);
                    bwprintf(COM2, "C %s ->", track->trackNodes[i].name);
                } else {
                    bwprintf(COM2, "Navigation Client - Error Pathfinding!!\n\r");
                }
                break;
            }
            default: {
                bwprintf(COM2, "%s ->", track->trackNodes[i].name);
                break;
            }
        }
    }
}

void navigationClient() {

    char trackIdentifier = bwgetc(COM2);
    bwputc(COM2, trackIdentifier);
    Track track = Track(trackIdentifier);

    char src[3] = {0};
    char dest[3] = {0};
    while (true) {
        src[0] = bwgetc(COM2);
        bwputc(COM2, src[0]);

        src[1] = bwgetc(COM2);
        bwputc(COM2, src[1]);

        dest[0] = bwgetc(COM2);
        bwputc(COM2, dest[0]);

        dest[1] = bwgetc(COM2);
        bwputc(COM2, dest[1]);

        bwprintf(COM2, "\n\r");
        djikstra(&track, src, dest);
    }

    Exit();
}
