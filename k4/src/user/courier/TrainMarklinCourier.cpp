#include "user/courier/TrainMarklinCourier.hpp"
#include "user/message/TRMessage.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)
void TrainMarklinCourier() {
    const int TRAIN_SERVER = WhoIs("TRAIN_SERVER");
    const int UART1_TX_SERVER = WhoIs("UART1TX");
    char replyMessage[10];
    FOREVER {
        // int replySize = Send(TRAIN_SERVER)
    }
}