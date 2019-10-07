#include "user/client/ClockClient.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "io/bwio.hpp"
#include "Constants.hpp"
#include "string.h"

void clockClient() {

    
    int parentTid = MyParentTid();
    int myTid = MyTid();

    char sendMessage[] = "a";
    char replyMessage[8];
    int replySize = Send(parentTid, sendMessage, 2, replyMessage, 8);
    
    int t;
    int n;
    memcpy(&t, replyMessage, sizeof(t));
    memcpy(&n, replyMessage + 4, sizeof(n));

    // bwprintf(COM2, "t: %d  n: %d\n\r", t, n);
    int clockServerTid = WhoIs("wCLOCK SERVER");

    for (int i = 1; i <= n; ++i) {
        int time = Delay(clockServerTid, t);
        bwprintf(COM2, "TID: %d  DELAY INTERVAL: %d  #delays: %d\n\r", myTid, time, i);
    }
    Exit();
}