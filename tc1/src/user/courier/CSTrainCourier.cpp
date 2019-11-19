#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/TermParseCourier.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void csTrainCourier() {
    int CS = WhoIs("CS");
    int TRAIN = WhoIs("TRAIN");
    int result;

    char trainmsg[16];
    MessageHeader *mh = (MessageHeader *)&trainmsg;
    RVMessage *rvmsg = (RVMessage *)&trainmsg;
    TRMessage *trmsg = (TRMessage *)&trainmsg;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // Get train message from command server
        result = Send(CS, (char *)&rdymsg, rdymsg.size(), trainmsg, 10);
        if (result < 0) {
            bwprintf(COM2, "CS->Train Courier - Send to Command Server failed\r\n");
        }
        if (mh->type == Constants::MSG::TR) {
            result = Send(TRAIN, (char*)trmsg, trmsg->size(), (char *)&rdymsg, rdymsg.size());
        } else if (mh->type == Constants::MSG::RV) {
            result = Send(TRAIN, (char*)rvmsg, rvmsg->size(), (char *)&rdymsg, rdymsg.size());
        } else {
            bwprintf(COM2, "CS->Train Courier - Expected TR or RV message but received unexpected message type %d\r\n", mh->type);
        }

        // Send train message to train server
        if (result < 0) {
            bwprintf(COM2, "CS->Train Courier - Send to Train Server failed\r\n");
        }
        if (rdymsg.mh.type != Constants::MSG::RDY) {
            bwprintf(COM2, "CS->Train Courier - Expected RDY message type but received unexpected message type %d\r\n", rdymsg.mh.type);
        }
    }
}
