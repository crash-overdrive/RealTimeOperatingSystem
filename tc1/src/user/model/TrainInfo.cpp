#include "user/model/TrainInfo.hpp"

TrainInfo::TrainInfo() : number{0}, speed{0}, next{0, 0}, prev{0, 0}, predictedTime{0}, predictedDistance{0}, timeDelta{0}, distanceDelta{0} {}
TrainInfo::TrainInfo(char n) : number{n}, speed{0}, next{0, 0}, prev{0, 0}, predictedTime{0}, predictedDistance{0}, timeDelta{0}, distanceDelta{0} {}
