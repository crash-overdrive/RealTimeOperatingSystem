#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/courier/CSSwitchCourier.hpp"
#include "user/courier/CSTrainCourier.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/server/CommandServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

void commandServer() {
    RegisterAs("CS");

    int result, tid;
    char msg[128];
    MessageHeader *mh = (MessageHeader*)&msg;
    TRMessage *trmsg = (TRMessage *)&msg;
    RVMessage *rvmsg = (RVMessage *)&msg;
    SWMessage *swmsg = (SWMessage *)&msg;
    // TextMessage *cmd = (TextMessage *)&msg;

    ThinMessage rdymsg(Constants::MSG::RDY);
    ThinMessage errmsg(Constants::MSG::ERR);

    int switchCourier = Create(5, csSwitchCourier);
    int trainCourier = Create(5, csTrainCourier);

    FOREVER {
        result = Receive(&tid, (char*)&msg, Constants::TrainCommandServer::MAX_SEND_MESSAGE_SIZE);
        if(result < 0) {
            // TODO: handle empty message
        }

        if (mh->type == Constants::MSG::RDY) {
            // Do nothing since commands should be coming from lower priority tasks! Otherwise we'd need to track which couriers are ready
        } else if (mh->type == Constants::MSG::TR) {
            bwprintf(COM2, "CS - TRAIN COMMAND RECEIVED!");
            Reply(trainCourier, msg, trmsg->size());
            Reply(tid, (char *)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::RV) {
            bwprintf(COM2, "CS - REVERSE COMMAND RECEIVED!");
            Reply(trainCourier, msg, rvmsg->size());
            Reply(tid, (char *)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::SW) {
            bwprintf(COM2, "CS - SWITCH COMMAND RECEIVED!");
            Reply(switchCourier, msg, swmsg->size());
            Reply(tid, (char *)&rdymsg, rdymsg.size());
        } else {
            bwprintf(COM2, "Command Server - Unrecognized message type received %d", mh->type);
        }
    }
}
