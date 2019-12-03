#ifndef SYSTEM_TIME_HPP
#define SYSTEM_TIME_HPP

class SystemTime{
    public:
        int minutes;
        int seconds;
        int tenthOfASecond;

        SystemTime();
        SystemTime(int mins, int secs, int tenthofsec);

        void setSystemTime(int ticksPassed);
};

#endif
