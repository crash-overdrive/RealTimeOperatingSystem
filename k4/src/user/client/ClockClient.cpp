#include "user/client/ClockClient.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "io/StringFormat.hpp"
#include "io/io.hpp"
#include "io/ts7200.h"
#include "Constants.hpp"
#include "string.h"

#define FOREVER for(;;)

class SystemTime {
    public:
        int minutes;
        int seconds;
        int tenthOfASecond;

        SystemTime() {
            minutes = 0;
            seconds = 0;
            tenthOfASecond = 0;
        }

        SystemTime(int mins, int secs, int tenthofsec) {
            minutes = mins;
            seconds = secs;
            tenthOfASecond = tenthofsec;
        }
};

SystemTime calculateSystemTime(int ticksPassed) {
	int milliSecondsElapsed = ticksPassed * 10;
	int secondsElapsed = milliSecondsElapsed / 1000;
	int tenthOfASecond = milliSecondsElapsed % 1000 / 100;

	SystemTime systemTime = SystemTime(secondsElapsed / 60, secondsElapsed % 60, tenthOfASecond);
	return systemTime;
}

void clockClient() {
    int clockServerTid = WhoIs("CLOCK SERVER");
    int UART2_TX = WhoIs("UART2TX");

    FOREVER {
        int time = Delay(clockServerTid, 10);
        SystemTime sysTime = calculateSystemTime(time);
        // char buffer[50];
        // int length = format(buffer, "\033[s\033[H%d:%d.%d\033[u", sysTime.minutes, sysTime.seconds, sysTime.tenthOfASecond);
        // for (int i = 0; i < length; ++i) {
        //     Putc(UART2_TX, UART2, buffer[i]);
        // }
    }
}
