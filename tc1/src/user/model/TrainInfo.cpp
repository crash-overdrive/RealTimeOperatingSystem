#include "user/model/TrainInfo.hpp"
#include "io/bwio.hpp"

TrainInfo::TrainInfo() : number{0}, speed{0}, next{0, 0}, prev{0, 0}, predictedTime{0}, predictedDist{0}, realTime{0}, realDist{0}  {}
