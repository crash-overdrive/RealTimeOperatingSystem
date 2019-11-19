#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/courier/SwitchMarklinCourier.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/server/SwitchServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

void switchServer() {
    RegisterAs("SWITCH");

    char states[22];

    int result, tid;
    char msg[16];
    MessageHeader *mh = (MessageHeader*)&msg;
    SWMessage *swmsg = (SWMessage *)&msg;

    ThinMessage rdymsg(Constants::MSG::RDY);
    ThinMessage errmsg(Constants::MSG::ERR);

    int marklinCourier = Create(5, switchMarklinCourier);
    bool marklinCourierReady = true;

    DataStructures::RingBuffer<SWMessage, 32> swbuf;

    FOREVER {
        result = Receive(&tid, (char*)&msg, 16);
        if(result < 0) {
            // TODO: handle empty message
        }

        if (mh->type == Constants::MSG::RDY) {
            if (swbuf.empty()) {
                marklinCourierReady = true;
            } else {
                *swmsg = swbuf.pop();
                int index = swmsg->sw > 18 ? swmsg->sw + 18 - 152 : swmsg->sw;
                states[index] = swmsg->state;
                Reply(marklinCourier, (char*)swmsg, swmsg->size());
            }
        } else if (mh->type == Constants::MSG::SW) {
            if (marklinCourierReady) {
                int index = swmsg->sw > 18 ? swmsg->sw + 18 - 152 : swmsg->sw;
                states[index] = swmsg->state;
                Reply(marklinCourier, msg, swmsg->size());
                marklinCourierReady = false;
            } else {
                swbuf.push(*swmsg);
            }
            Reply(tid, (char *)&rdymsg, rdymsg.size());
        } else {
            bwprintf(COM2, "Switch Server - Unrecognized message type received %d", mh->type);
        }
    }
}
