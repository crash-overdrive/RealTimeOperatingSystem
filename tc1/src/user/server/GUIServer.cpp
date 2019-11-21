#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/StringFormat.hpp"
#include "io/VT100.hpp"
#include "user/courier/GUITermCourier.hpp"
#include "user/message/IdleMessage.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/SensorMessage.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/TimeMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/model/Train.hpp"
#include "user/server/GUIServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for (;;)

const char *BORDER_CHARS = "═║╔╗╚╝╠╣╦╩╬";

int GUI::insertSetDisplayAttrs(char *str, int attr) {
    char attrstr[6] = {0};
    format(attrstr, Constants::VT100::SET_ATTR, attr);
    return format(str, "%s", attrstr);
}

int GUI::insertSaveCursorAndAttrs(char *str, int index) {
    str[index++] = '\033';
    str[index++] = '7';
    return index;
}

int GUI::insertRestorCursorAndAttrs(char *str, int index) {
    str[index++] = '\033';
    str[index++] = '8';
    return index;
}

// Inserts a force cursor position escape sequence at the desired location in the string
int GUI::insertForceCursorPos(char *str, int index, int row, int col) {
    str[index++] = '\033';
    str[index++] = '[';
    // Logic for inserting row
    str[index++] = ';';
    // Logic for inserting col
    str[index++] = 'f';
    return index;
}

void GUI::drawTime(char *msg) {
    TimeMessage *tm = (TimeMessage *)msg;
    drawmsg.msglen = 0;
    drawmsg.msglen += format(drawmsg.msg, "%s%s", Constants::VT100::SAVE_CURSOR_AND_ATTRS, Constants::VT100::MOVE_CURSOR_POS_TO_TIME);
    if (tm->m / 10 == 0) {
        drawmsg.msg[drawmsg.msglen++] = '0';
    }
    drawmsg.msglen += format(&drawmsg.msg[drawmsg.msglen], "%d:", tm->m);
    if (tm->s / 10 == 0) {
        drawmsg.msg[drawmsg.msglen++] = '0';
    }
    drawmsg.msglen += format(&drawmsg.msg[drawmsg.msglen], "%d.%d%s", tm->s, tm->ms, Constants::VT100::RESTORE_CURSOR_AND_ATTRS);
    drawmsg.msg[drawmsg.msglen] = 0;
    Reply(termCourier, (char*)&drawmsg, drawmsg.size());
}

void GUI::drawIdle(char *msg) {
    IdleMessage *im = (IdleMessage *)msg;
    drawmsg.msglen = 0;
    drawmsg.msglen += format(drawmsg.msg, "%s%s", Constants::VT100::SAVE_CURSOR_AND_ATTRS, Constants::VT100::MOVE_CURSOR_POS_TO_IDLE);
    if (im->integer >= 80) {
        drawmsg.msglen += insertSetDisplayAttrs(&drawmsg.msg[drawmsg.msglen], FG_GREEN);
    } else if (im->integer >= 50) {
        drawmsg.msglen += insertSetDisplayAttrs(&drawmsg.msg[drawmsg.msglen], FG_YELLOW);
    } else {
        drawmsg.msglen += insertSetDisplayAttrs(&drawmsg.msg[drawmsg.msglen], FG_RED);
    }
    if (im->integer / 10 == 0) {
        drawmsg.msg[drawmsg.msglen++] = '0';
    }
    drawmsg.msglen += format(&drawmsg.msg[drawmsg.msglen], "%d.", im->integer);
    if (im->fractional / 10 == 0) {
        drawmsg.msg[drawmsg.msglen++] = '0';
    }
    drawmsg.msglen += format(&drawmsg.msg[drawmsg.msglen], "%d%s", im->fractional, Constants::VT100::RESTORE_CURSOR_AND_ATTRS);
    Reply(termCourier, (char*)&drawmsg, drawmsg.size());
}

void GUI::drawSensors(char *msg) {
    SensorMessage *sm = (SensorMessage *)msg;

    drawmsg.msglen = 0;
    drawmsg.msglen += format(drawmsg.msg, "%s%s", Constants::VT100::SAVE_CURSOR_AND_ATTRS, Constants::VT100::MOVE_CURSOR_POS_TO_SENSOR);
    // This is a tiny bit hacky because the sensor message should probably provide length, but that's a problem for another day
    // TODO(sgaweda): Fix this another day!
    for (int i = 9; i >= 0; --i) {
        // We check here to see if the bank isn't a valid bank
        if (sm->sensorData[i].sensor.bank > 0) {
            if (sm->sensorData[i].train == 1) {
                drawmsg.msglen += insertSetDisplayAttrs(&drawmsg.msg[drawmsg.msglen], FG_RED);
            } else {
                drawmsg.msglen += insertSetDisplayAttrs(&drawmsg.msg[drawmsg.msglen], FG_BLUE);
            }
            if (sm->sensorData[i].sensor.number / 10 == 0) {
                drawmsg.msglen += format(&drawmsg.msg[drawmsg.msglen], "%c0%d ", sm->sensorData[i].sensor.bank, sm->sensorData[i].sensor.number);
            } else {
                drawmsg.msglen += format(&drawmsg.msg[drawmsg.msglen], "%c%d ", sm->sensorData[i].sensor.bank, sm->sensorData[i].sensor.number);
            }
        }
    }
    drawmsg.msglen += format(&drawmsg.msg[drawmsg.msglen], "%s", Constants::VT100::RESTORE_CURSOR_AND_ATTRS);
    Reply(termCourier, (char*)&drawmsg, drawmsg.size());
}

void GUI::drawSwitch(char *msg) {
    char posstr[8] = {0};
    SWMessage *sw = (SWMessage *)msg;
    int xpos = 6;
    int ypos = 1;
    if (sw->sw >= 153) {
        ypos += (sw->sw - 152 + 18) * 4;
    } else {
        ypos += sw->sw * 4;
    }
    format(posstr, Constants::VT100::MOVE_CURSOR_POS, xpos, ypos);

    drawmsg.msglen = 0;
    drawmsg.msglen += format(drawmsg.msg, "%s%s%c%s", Constants::VT100::SAVE_CURSOR_AND_ATTRS, posstr, sw->state, Constants::VT100::RESTORE_CURSOR_AND_ATTRS);
    drawmsg.msg[drawmsg.msglen] = 0;
    Reply(termCourier, (char*)&drawmsg, drawmsg.size());
}

void GUI::drawTrain(char *msg) {
    // TODO: implement me
}

void GUI::init() {
    // style = Constants::GUIServer::STYLE::B;
    // switch(style) {
    // case Constants::GUIServer::STYLE::A:
    //     base = baseGUIa;
    //     baseWidth = baseGUIaWidth;
    //     break;
    // case Constants::GUIServer::STYLE::B:
    //     base = baseGUIb;
    //     baseWidth = baseGUIbWidth;
    //     break;
    // case Constants::GUIServer::STYLE::C:
    //     base = baseGUIc;
    //     baseWidth = baseGUIcWidth;
    //     break;
    // default:
    //     bwprintf(COM2, "GUI Server - Invalid GUI style!");
    //     break;
    // }
    termCourier = Create(7, guiTermCourier);
    // drawingBase = true;
}

void guiServer() {
    RegisterAs("GUI");
    GUI gui;
    gui.init();

    int result, tid;
    char msg[128];
    MessageHeader *mh = (MessageHeader *)msg;
    // SensorMessage *sm = (SensorMessage *)msg;
    // TimeMessage *tm = (TimeMessage *)msg;
    ThinMessage rdymsg(Constants::MSG::RDY);

    bool tsBlocked = false;

    FOREVER {
        result = Receive(&tid, msg, 128);
        if (result < 0) {
            // TODO: handle receive error
        }

        // All couriers sending messages to this server should be priority 8 (except the terminal server)
        switch (mh->type) {
            case Constants::MSG::RDY:
                // TODO(sgaweda): reply only when ready to draw!
                tsBlocked = true;
                break;
            case Constants::MSG::TIME:
                if (tsBlocked != true) {
                    bwprintf(COM2, "GUI Server - Courier unexpectedly blocked!");
                }
                gui.drawTime(msg);
                Reply(tid, (char*)&rdymsg, rdymsg.size());
                tsBlocked = false;
                break;
            case Constants::MSG::IDLE:
                if (tsBlocked != true) {
                    bwprintf(COM2, "GUI Server - Courier unexpectedly blocked!");
                }
                gui.drawIdle(msg);
                Reply(tid, (char*)&rdymsg, rdymsg.size());
                tsBlocked = false;
                break;
            case Constants::MSG::SENSOR:
                if (tsBlocked != true) {
                    bwprintf(COM2, "GUI Server - Courier unexpectedly blocked!");
                }
                gui.drawSensors(msg);
                Reply(tid, (char*)&rdymsg, rdymsg.size());
                tsBlocked = false;
                break;
            case Constants::MSG::SW:
                if (tsBlocked != true) {
                    bwprintf(COM2, "GUI Server - Courier unexpectedly blocked!");
                }
                gui.drawSwitch(msg);
                Reply(tid, (char*)&rdymsg, rdymsg.size());
                tsBlocked = false;
                break;
            case Constants::MSG::TRAIN: // TODO(sgaweda): This case will remain unimplemented for now
                gui.drawTrain(msg);
                Reply(tid, (char*)&rdymsg, rdymsg.size());
                tsBlocked = false;
                break;
            default:
                bwprintf(COM2, "GUI Server - Unrecognized message type received");
                break;
        }
    }
}

// These are preserved for reference

// int baseGUIaWidth = 82;
// const char *baseGUIa =
// "╔═════════════════════════════════════════════════════════╦═════════╦══════════╗\r\n"
// "║ Time                                                    ║ Sensors ║ Switches ║\r\n"
// "╠═════════════════════════════════════════════════════════╣         ║    1     ║\r\n"
// "║ Idle                                                    ║         ║    2     ║\r\n"
// "╠═════════════════════════════════════════════════════════╣         ║    3     ║\r\n"
// "║ Sensor Predictions                                      ║         ║    4     ║\r\n"
// "║                                                         ║         ║    5     ║\r\n"
// "║ Next                                                    ║         ║    6     ║\r\n"
// "║ Time Pred                                               ║         ║    7     ║\r\n"
// "║ Dist Pred                                               ║         ║    8     ║\r\n"
// "║                                                         ║         ║    9     ║\r\n"
// "║ Last                                                    ║         ║   10     ║\r\n"
// "║ Time Real                                               ║         ║   11     ║\r\n"
// "║ Dist Real                                               ║         ║   12     ║\r\n"
// "║                                                         ║         ║   13     ║\r\n"
// "║                                                         ║         ║   14     ║\r\n"
// "║                                                         ║         ║   15     ║\r\n"
// "║                                                         ║         ║   16     ║\r\n"
// "║                                                         ║         ║   17     ║\r\n"
// "║                                                         ║         ║   18     ║\r\n"
// "║                                                         ║         ║  153     ║\r\n"
// "║                                                         ║         ║  154     ║\r\n"
// "║                                                         ║         ║  155     ║\r\n"
// "║                                                         ║         ║  156     ║\r\n"
// "╠═════════════════════════════════════════════════════════╩═════════╩══════════║\r\n"
// "║ :)                                                                           ║\r\n"
// "║                                                                              ║\r\n"
// "╠══════════════════════════════════════════════════════════════════════════════╣\r\n"
// "║ Message Log                                                                  ║\r\n"
// "║                                                                              ║\r\n"
// "║                                                                              ║\r\n"
// "║                                                                              ║\r\n"
// "║                                                                              ║\r\n"
// "╚══════════════════════════════════════════════════════════════════════════════╝\r\n";

// int baseGUIbWidth = 269;
// const char *baseGUIb =
// "╔══════════════╦══════════════╦═════════════════════════════════════════════════════════╗\r\n"
// "║ Time MM:SS.S ║ Idle: PP.PP% ║                                                         ║\r\n"
// "╠══════════════╩══════════════╩═════════════════════════════════════════════════════════╣\r\n"
// "║ Switches                                                                              ║\r\n"
// "║   1   2   3   4   5   6   7   8   9  10  12  13  14  15  16  17  18 153 154 155 156   ║\r\n"
// "║                                                                                       ║\r\n"
// "╠═══════════════════════════════════════════════════════════════════════════════════════╣\r\n"
// "║ Sensors                                                                               ║\r\n"
// "║   [ ]                                                                                 ║\r\n"
// "╠═══════════════════════════════════════════════════════════════════════════════════════╣\r\n"
// "║ Sensor Predictions                                                                    ║\r\n"
// "║                                                                                       ║\r\n"
// "║   Next                                                                                ║\r\n"
// "║   Time Pred                                                                           ║\r\n"
// "║   Dist Pred                                                                           ║\r\n"
// "║                                                                                       ║\r\n"
// "║   Last                                                                                ║\r\n"
// "║   Time Real                                                                           ║\r\n"
// "║   Dist Real                                                                           ║\r\n"
// "╠═══════════════════════════════════════════════════════════════════════════════════════║\r\n"
// "║ :)                                                                                    ║\r\n"
// "║                                                                                       ║\r\n"
// "╠═══════════════════════════════════════════════════════════════════════════════════════╣\r\n"
// "║ Message Log                                                                           ║\r\n"
// "║                                                                                       ║\r\n"
// "║                                                                                       ║\r\n"
// "║                                                                                       ║\r\n"
// "║                                                                                       ║\r\n"
// "╚═══════════════════════════════════════════════════════════════════════════════════════╝\r\n";

// int baseGUIcWidth = 91;
// const char *baseGUIc =
// "+=======================================================================================+\r\n"
// "| Time MM:SS.S | Idle: PP.PP% |                                                         |\r\n"
// "|=======================================================================================|\r\n"
// "| Switches                                                                              |\r\n"
// "|   1   2   3   4   5   6   7   8   9  10  12  13  14  15  16  17  18 153 154 155 156   |\r\n"
// "|                                                                                       |\r\n"
// "|=======================================================================================|\r\n"
// "| Sensors                                                                               |\r\n"
// "|   [ ]                                                                                 |\r\n"
// "|=======================================================================================|\r\n"
// "| Sensor Predictions                                                                    |\r\n"
// "|                                                                                       |\r\n"
// "|   Next                                                                                |\r\n"
// "|   Time Pred                                                                           |\r\n"
// "|   Dist Pred                                                                           |\r\n"
// "|                                                                                       |\r\n"
// "|   Last                                                                                |\r\n"
// "|   Time Real                                                                           |\r\n"
// "|   Dist Real                                                                           |\r\n"
// "|=======================================================================================|\r\n"
// "| :)                                                                                    |\r\n"
// "|                                                                                       |\r\n"
// "|=======================================================================================|\r\n"
// "| Message Log                                                                           |\r\n"
// "|                                                                                       |\r\n"
// "|                                                                                       |\r\n"
// "|                                                                                       |\r\n"
// "|                                                                                       |\r\n"
// "+=======================================================================================+\r\n";
