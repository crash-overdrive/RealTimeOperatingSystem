#include "user/model/Train.hpp"
#include "io/bwio.hpp"

Train::Train() : number{0}, speed{0} {}

Train::Train(char n) : number{n}, speed{0} {}

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
            bwprintf(COM2, "static getTrainIndex() - Request for invalid train index! %d", number);
            return TRINDEX::T1;
    }
}
