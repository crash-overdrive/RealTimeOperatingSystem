#include "user/model/SystemTime.hpp"

SystemTime::SystemTime() {
    minutes = 0;
    seconds = 0;
    tenthOfASecond = 0;
}

SystemTime::SystemTime(int mins, int secs, int tenthofsec) {
    minutes = mins;
    seconds = secs;
    tenthOfASecond = tenthofsec;
}

void SystemTime::setSystemTime(int ticksPassed) {
    int milliSecondsElapsed = ticksPassed * 10;
    int secondsElapsed = milliSecondsElapsed / 1000;

    minutes = secondsElapsed / 60;
    seconds = secondsElapsed % 60;
    tenthOfASecond = milliSecondsElapsed % 1000 / 100;
}
