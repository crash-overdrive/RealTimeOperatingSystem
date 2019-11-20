#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/courier/SwitchGUICourier.hpp"
#include "user/courier/SwitchMarklinCourier.hpp"
// #include "user/message/SWMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/server/SwitchServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

void SwitchServer::sendMarklin() {
    if (marklinCourierReady && !swbuf.empty()) {
        *swmsg = swbuf.pop();
        int index = swmsg->sw > 18 ? swmsg->sw + 18 - 153 : swmsg->sw - 1;
        states[index] = swmsg->state;
        guibuf.push(*swmsg);
        marklinCourierReady = false;
        Reply(marklinCourier, (char*)swmsg, swmsg->size());
    }
}

void SwitchServer::sendGUI() {
    if (guiCourierReady && !guibuf.empty()) {
        *swmsg = guibuf.pop();
        guiCourierReady = false;
        Reply(guiCourier, (char*)swmsg, swmsg->size());
    }
}

void SwitchServer::init() {
    marklinCourier = Create(5, switchMarklinCourier);
    guiCourier = Create(8, switchGUICourier);
    marklinCourierReady = false;
    guiCourierReady = false;
}

void switchServer() {
    RegisterAs("SWITCH");

    SwitchServer sws;
    sws.init();

    int result, tid;

    ThinMessage rdymsg(Constants::MSG::RDY);
    ThinMessage errmsg(Constants::MSG::ERR);

    FOREVER {
        result = Receive(&tid, (char*)&sws.msg, 16);
        if (result < 0) {
            // TODO: handle empty message
        }

        if (sws.mh->type == Constants::MSG::RDY) {
            // Find out which service is ready and react appropriately
            if (tid == sws.marklinCourier) {
                sws.marklinCourierReady = true;
            } else if (tid == sws.guiCourier) {
                sws.guiCourierReady = true;
            }
            sws.sendMarklin();
            sws.sendGUI();
        } else if (sws.mh->type == Constants::MSG::SW) {
            sws.swbuf.push(*sws.swmsg);
            sws.sendMarklin();
            sws.sendGUI();
            Reply(tid, (char *)&rdymsg, rdymsg.size());
        } else {
            bwprintf(COM2, "Switch Server - Unrecognized message type received %d", sws.mh->type);
        }
    }
}
