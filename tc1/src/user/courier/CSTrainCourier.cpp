#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/courier/TermParseCourier.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

void csTrainCourier() {
    int CS = WhoIs("CS");
    // int TRAIN = WhoIs("TRAIN");
    int result;

    char trainmsg[10];
    MessageHeader *mh = (MessageHeader *)&trainmsg;
    ThinMessage rdymsg(Constants::MSG::RDY);

    FOREVER {
        // Get train message from command server
        result = Send(CS, (char *)&rdymsg, rdymsg.size(), trainmsg, 10);
        if (result < 0) {
            bwprintf(COM2, "CS->Train Courier - Send to Command Server failed\r\n");
        }
        if (mh->type != Constants::MSG::TR && mh->type != Constants::MSG::RV) {
            bwprintf(COM2, "CS->Train Courier - Expected TR or RV message but received unexpected message type %d\r\n", mh->type);
        }

        // Send train message to train server
        // result = Send(TRAIN, trainmsg, 10, (char *)&rdymsg, rdymsg.size());
        // if (result < 0) {
        //     bwprintf(COM2, "CS->Train Courier - Send to Train Server failed\r\n");
        // }
        // if (rdymsg.mh.type != Constants::MSG::RDY) {
        //     bwprintf(COM2, "CS->Train Courier - Expected RDY message type but received unexpected message type %d\r\n", rdymsg.mh.type);
        // }
    }
}
