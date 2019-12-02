#include "io/bwio.hpp"
#include "user/model/Train.hpp"
#include "user/syscall/UserSyscall.hpp"

Train::Train() : number{0}, speed{0} {}

Train::Train(char n) : number{n}, speed{0}, headlights{false}, reverse{false}, trainInfo{n}, updated{false} {}

TRINDEX Train::getTrainIndex(char number) {
    switch (number) {
        case 1:
            return TRINDEX::T1;
        case 24:
            return TRINDEX::T24;
        case 58:
            return TRINDEX::T58;
        case 74:
            return TRINDEX::T74;
        case 78:
            return TRINDEX::T78;
        default:
            bwprintf(COM2, "static getTrainIndex() - Request for invalid train index! Train number: %d\r\n Task ID: %d", number, MyTid());
            return TRINDEX::T1;
    }
}

char Train::getTrainNumber(char index) {
    switch (index) {
        case TRINDEX::T1:
            return 1;
        case TRINDEX::T24:
            return 24;
        case TRINDEX::T58:
            return 58;
        case TRINDEX::T74:
            return 74;
        case TRINDEX::T78:
            return 78;
        default:
            bwprintf(COM2, "static getTrainNumber() - Request for invalid train number! Train index: %d Task ID: %d", index, MyTid());
            return 1;
    }
}

const int Train::accelerations[5][15] = {
    { 0, 0, 0, 0, 0, 0, 544500, 1044321, 1221025, 1297350, 1305390, 1468878, 1475994, 1515433, 1400142},
    { 0, 0, 0, 0, 0, 0, 544500, 1044321, 1221025, 1297350, 1305390, 1468878, 1475994, 1515433, 1400142},
    { 0, 0, 0, 0, 0, 0, 544500, 1044321, 1221025, 1297350, 1305390, 1468878, 1475994, 1515433, 1400142},
    { 0, 0, 0, 0, 0, 0, 544500, 1044321, 1221025, 1297350, 1305390, 1468878, 1475994, 1515433, 1400142},
    { 0, 0, 0, 0, 0, 0, 544500, 1044321, 1221025, 1297350, 1305390, 1468878, 1475994, 1515433, 1400142},
};

const int Train::velocities[5][15] = {
    { 0, 20000, 0, 0, 0, 0, 99000, 171000, 221000, 279000, 337000, 411000, 478000, 545000, 594000 },
    { 0, 0, 0, 0, 0, 0, 99000, 171000, 221000, 279000, 337000, 411000, 478000, 545000, 594000 },
    { 0, 0, 0, 0, 0, 0, 99000, 171000, 221000, 279000, 337000, 411000, 478000, 545000, 594000 },
    { 0, 0, 0, 0, 0, 0, 99000, 171000, 221000, 279000, 337000, 411000, 478000, 545000, 594000 },
    { 0, 0, 0, 0, 0, 0, 99000, 171000, 221000, 279000, 337000, 411000, 478000, 545000, 594000 },
};

const int Train::decelerations[5][15] = {
    { 0, 0, 0, 0, 0, 0, 544500, 1044321, 1221025, 1297350, 1305390, 1468878, 1475994, 1515433, 1400142},
    { 0, 0, 0, 0, 0, 0, 544500, 1044321, 1221025, 1297350, 1305390, 1468878, 1475994, 1515433, 1400142},
    { 0, 0, 0, 0, 0, 0, 544500, 1044321, 1221025, 1297350, 1305390, 1468878, 1475994, 1515433, 1400142},
    { 0, 0, 0, 0, 0, 0, 544500, 1044321, 1221025, 1297350, 1305390, 1468878, 1475994, 1515433, 1400142},
    { 0, 0, 0, 0, 0, 0, 544500, 1044321, 1221025, 1297350, 1305390, 1468878, 1475994, 1515433, 1400142},
};
