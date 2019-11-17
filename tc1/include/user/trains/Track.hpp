#ifndef TRACK_HPP
#define TRACK_HPP

#define DIR_AHEAD 0
#define DIR_STRAIGHT 0
#define DIR_CURVED 1

// The track initialization functions expect an array of this size.
#define TRACK_MAX 144

enum NODE_TYPE {
    NODE_NONE,
    NODE_SENSOR,
    NODE_BRANCH,
    NODE_MERGE,
    NODE_ENTER,
    NODE_EXIT,
};

class TrackNode;

class TrackEdge {
    public:
        TrackEdge *reverseEdge;
        TrackNode *srcNode;
        TrackNode *destNode;
        int dist;             /* in millimetres */
};

class TrackNode {
    public:
        const char *name;
        NODE_TYPE type;
        int num;              /* sensor or switch number */
        TrackNode *reverseNode;  /* same location, but opposite direction */
        TrackEdge edges[2];
};

class Track {
    private:
        void initializeTrack(char trackIdentifier); // trackIdentifier should be 'A' or 'B'
    public:
        TrackNode trackNodes[TRACK_MAX];
        int noOfNodes;

        Track(char trackIdentifier) {
            initializeTrack(trackIdentifier);
        }
};

#endif
