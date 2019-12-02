#include "io/bwio.hpp"
#include "user/model/Train.hpp"
#include "user/syscall/UserSyscall.hpp"

Train::Train() : number{0}, speed{0} {}

Train::Train(char n) : number{n}, speed{0}, headlights{false}, reverse{false}, trainInfo{n}, updated{false}, aheadOfPrediction{false, false} {}

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
    {0, 0, 0, 0, 0, 0, 54450, 104432, 122102, 129735, 132056, 148176, 148366, 151543, 140014},
    {0, 0, 0, 0, 0, 0, 61004, 114469, 136242, 151250, 162526, 176269, 175662, 152889, 151044},
    {0, 0, 0, 0, 0, 0, 54450, 104432, 122102, 129735, 132056, 148176, 148366, 151543, 140014},
    {0, 0, 0, 0, 0, 0, 54450, 104432, 122102, 129735, 132056, 148176, 148366, 151543, 140014},
    {0, 0, 0, 0, 0, 0, 54450, 104432, 122102, 129735, 132056, 148176, 148366, 151543, 140014},
};

const int Train::velocities[5][15] = {
    {0, 0, 0, 0, 0, 0, 99000, 171000, 221000, 279000, 337000, 411000, 478000, 545000, 594000},
    {0, 0, 0, 0, 0, 0, 121000, 203000, 261000, 330000, 395000, 475000, 540000, 572000, 648000},
    {0, 0, 0, 0, 0, 0, 99000, 171000, 221000, 279000, 337000, 411000, 478000, 545000, 594000},
    {0, 0, 0, 0, 0, 0, 99000, 171000, 221000, 279000, 337000, 411000, 478000, 545000, 594000},
    {0, 0, 0, 0, 0, 0, 99000, 171000, 221000, 279000, 337000, 411000, 478000, 545000, 594000},
};

const int Train::decelerations[5][15] = {
    {0, 0, 0, 0, 0, 0, 54450, 104432, 122102, 129735, 132056, 148176, 148366, 151543, 140014},
    {0, 0, 0, 0, 0, 0, 61004, 114469, 136242, 151250, 162526, 176269, 175662, 152889, 151044},
    {0, 0, 0, 0, 0, 0, 54450, 104432, 122102, 129735, 132056, 148176, 148366, 151543, 140014},
    {0, 0, 0, 0, 0, 0, 54450, 104432, 122102, 129735, 132056, 148176, 148366, 151543, 140014},
    {0, 0, 0, 0, 0, 0, 54450, 104432, 122102, 129735, 132056, 148176, 148366, 151543, 140014},
};

const int Train::stoppingDistances[5][15] = {
    {0, 0, 0, 0, 0, 0, 90000, 140000, 200000, 300000, 430000, 570000, 770000, 980000, 1260000},
    {0, 0, 0, 0, 0, 0, 120000, 180000, 250000, 360000, 480000, 640000, 830000, 1070000, 1390000},
    {0, 0, 0, 0, 0, 0, 90000, 140000, 200000, 300000, 430000, 570000, 770000, 980000, 1260000},
    {0, 0, 0, 0, 0, 0, 90000, 140000, 200000, 300000, 430000, 570000, 770000, 980000, 1260000},
    {0, 0, 0, 0, 0, 0, 90000, 140000, 200000, 300000, 430000, 570000, 770000, 980000, 1260000},
};
