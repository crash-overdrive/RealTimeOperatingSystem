#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "kern/BootLoader.hpp"
#include "user/client/ClockClient.hpp"
#include "user/client/IdleTask.hpp"
#include "user/client/SensorData.hpp"
#include "user/client/Test.hpp"
#include "user/client/TrackInit.hpp"
#include "user/server/ClockServer.hpp"
#include "user/server/CommandServer.hpp"
#include "user/server/GUIServer.hpp"
#include "user/server/MarklinServer.hpp"
#include "user/server/NameServer.hpp"
#include "user/server/ParseServer.hpp"
#include "user/server/SwitchServer.hpp"
#include "user/server/TrainServer.hpp"
#include "user/server/TerminalServer.hpp"
#include "user/server/UART1RXServer.hpp"
#include "user/server/UART1TXServer.hpp"
#include "user/server/UART2RXServer.hpp"
#include "user/server/UART2TXServer.hpp"
#include "user/syscall/UserSyscall.hpp"

// #include "user/message/TextMessage.hpp"
// #include "user/message/TRMessage.hpp"

namespace Constants {
    namespace NameServer {
        int TID = -1;
    }
    namespace ClockServer {
        int TID = -1;
    }
    namespace UART1RXServer {
        volatile int TID = -1;
    }
    namespace UART1TXServer {
        volatile int TID = -1;
    }
    namespace UART2RXServer {
        volatile int TID = -1;
    }
    namespace UART2TXServer {
        volatile int TID = -1;
    }
}
void bootLoader() {
    int tid;

    tid = Create(2, nameServer);
    Constants::NameServer::TID = tid;
    // bwprintf(COM2, "BootLoader - Created Name Server with tid: %d\n\r", tid);

    tid = Create(1, clockServer);
    // bwprintf(COM2, "BootLoader - Created Clock Server with tid: %d\n\r", tid);
    Constants::ClockServer::TID = tid;
    // int ctid = tid;

    tid = Create(3, uart1rxServer);
    // bwprintf(COM2, "BootLoader - Created UART1RX Server with tid: %d\n\r", tid);
    Constants::UART1RXServer::TID = tid;

    tid = Create(4, uart1txServer);
    // bwprintf(COM2, "BootLoader - Created UART2TX Server with tid: %d\n\r", tid);
    Constants::UART1TXServer::TID = tid;

    tid = Create(5, marklinServer);
    // int mstid = tid;

    tid = Create(5, uart2rxServer);
    // bwprintf(COM2, "BootLoader - Created UART2RX Server with tid: %d\n\r", tid);
    Constants::UART2RXServer::TID = tid;

    tid = Create(6, uart2txServer);
    // bwprintf(COM2, "BootLoader - Created UART2TX Server with tid: %d\n\r", tid);
    Constants::UART2TXServer::TID = tid;

    tid = Create(7, parseServer);
    tid = Create(7, commandServer);
    tid = Create(5, switchServer);
    tid = Create(5, trainServer);
    // int tcstid = tid;
    // bwprintf(COM2, "BootLoader - Created Train Controller with tid: %d\n\r", tid);

    tid = Create(7, terminalServer);
    // int tstid = tid;
    // bwprintf(COM2, "BootLoader - Created Terminal Server with tid: %d\n\r", tid);

    tid = Create(7, guiServer);
    // int guitid = tid;
    // bwprintf(COM2, "BootLoader - Created GUI Server with tid: %d\n\r", tid);

    tid = Create(7, trackInit);

    // TODO(sgaweda): Remove this when we are sure that Sensor Data should be created by Marklin Server
    // tid = Create(8, sensorData);
    // bwprintf(COM2, "BootLoader - Sensor Data client created\r\n");

    tid = Create(9, clockClient);

    // Create the system's idle task
    tid = Create(Constants::NUM_PRIORITIES-1, idleTask);

    // TextMessage textmsg;
    // char msg[17] = "this is a test\r\n";
    // for (int i = 0; i < 17; ++i) {
    //     textmsg.msg[i] = msg[i];
    // }
    // textmsg.msglen = 17;

    // Send(tstid, (char*)&textmsg, textmsg.size(), "reply", 6);

    // tid = Create(10, testTCStr);
    // tid = Create(10, testTCSsw);

    // TRMessage trmsg;
    // trmsg.train = 1;
    // trmsg.speed = 10;
    // trmsg.headlights = true;
    // Send(mstid, (char*)&trmsg, trmsg.size(), "reply", 6);
    // bwprintf(COM2, "Bootloader - Exiting");

    Exit();
}
