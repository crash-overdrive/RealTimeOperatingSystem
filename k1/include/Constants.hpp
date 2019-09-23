#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {
    const int NUM_PRIORITIES = 8;
    const int Q_LENGTH = 4;
    const int KIBIBYTE = 262144;
    const enum STATE {
        ACTIVE,
        READY,
        BLOCKED
    };
};

#endif