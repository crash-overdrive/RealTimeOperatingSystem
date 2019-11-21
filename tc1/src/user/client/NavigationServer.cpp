#include <string.h>

#include "Constants.hpp"
#include "data-structures/Stack.hpp"
#include "io/bwio.hpp"
#include "io/StringFormat.hpp"
#include "user/client/NavigationServer.hpp"
#include "user/trains/Track.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

int nextNodeToBeConsidered(Track* track, bool visitedNodes[], int dist[]) {
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

bool djikstraFinished(Track* track, bool visitedNodes[]) {
    int noOfNodes = track->noOfNodes;
    for (int i = 0; i < noOfNodes; ++i) {
        if (visitedNodes[i] == false && track->trackNodes[i].reserved == false) {
            return false;
        }
    }
    return true;
}

void djikstra(Track* track, int srcIndex, int destIndex, int lastHop[]) {
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

void navigationServer() {
    RegisterAs("NAV");
    Track track = Track('A');
    DataStructures::Stack<int, TRACK_MAX> path;
    int lastHop[track.noOfNodes];
    char src[10] = {0};
    char dest[10] = {0};
    int tid;

    FOREVER {
        Receive(&tid, src, 10);
        Reply(tid, &Constants::Server::ACK, 1);
        Receive(&tid, dest, 10);
        Reply(tid, &Constants::Server::ACK, 1);

        int srcIndex = -1;
        int destIndex = -1;
        path.reset();

        for(int i = 0; i < track.noOfNodes; ++i) {
            if (!strcmp(track.trackNodes[i].name, src)) {
                srcIndex = i;
            } else if(!strcmp(track.trackNodes[i].name, dest)) {
                destIndex = i;
            }
        }

        if (srcIndex == -1 || destIndex == -1) {
            bwprintf(COM2, "Navigation Server - Bad src %s  or dest %s \n\r", src, dest);
        }

        djikstra(&track, srcIndex, destIndex, lastHop);

        int index = destIndex;
        path.push(index);
        while (index != srcIndex) {
            index = lastHop[index];
            if (index == -1) {
                bwprintf(COM2, "Navigation Server - No Path Exists from %s to %s\n\r", src, dest);
                path.reset();
                break;
            }
            path.push(index);
        }

        while(!path.empty()) {
            int i = path.pop();
            track.trackNodes[i].reserved = true;
            track.trackNodes[i].reverseNode->reserved = true;

            switch(track.trackNodes[i].type) {
                case NODE_TYPE::NODE_BRANCH:
                {
                    const char* nextNodeName = track.trackNodes[path.peek()].name;
                    const char* straightDirectionName = track.trackNodes[i].edges[DIR_STRAIGHT].destNode->name;
                    const char* curvedDirectionName = track.trackNodes[i].edges[DIR_CURVED].destNode->name;

                    if (!strcmp(nextNodeName, straightDirectionName)) {
                        bwprintf(COM2, "Straight SW %s -> ", track.trackNodes[i].name);
                    } else if(!strcmp(nextNodeName, curvedDirectionName)) {
                        bwprintf(COM2, "Curved SW %s -> ", track.trackNodes[i].name);
                    } else {
                        bwprintf(COM2, "Navigation Client - Error Pathfinding!!\n\r");
                    }
                    break;
                }
                case NODE_TYPE::NODE_SENSOR:
                case NODE_TYPE::NODE_MERGE:
                {
                    const char* nextNodeName = track.trackNodes[path.peek()].name;
                    const char* aheadDirectionName = track.trackNodes[i].edges[DIR_AHEAD].destNode->name;
                    const char* reverseDirectionName = track.trackNodes[i].reverseNode->name;
                    if (!strcmp(nextNodeName, aheadDirectionName)) {
                        bwprintf(COM2, "Straight %s -> ", track.trackNodes[i].name);
                    } else if(!strcmp(nextNodeName, reverseDirectionName)) {
                        bwprintf(COM2, "Reverse %s -> ", track.trackNodes[i].name);
                    } else {
                        bwprintf(COM2, track.trackNodes[i].name);
                    }
                    break;
                }
                default: {
                    bwprintf(COM2, "%s ->", track.trackNodes[i].name);
                    break;
                }
            }
        }
    }
}
