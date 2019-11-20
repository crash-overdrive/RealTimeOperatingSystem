#ifndef SWITCH_SERVER_HPP
#define SWITCH_SERVER_HPP

#include "data-structures/RingBuffer.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/message/SWMessage.hpp"

void switchServer();

class SwitchServer {
public:
    char states[22];

    char msg[16];
    MessageHeader *mh = (MessageHeader*)&msg;
    SWMessage *swmsg = (SWMessage *)&msg;

    int marklinCourier;
    int guiCourier;
    bool marklinCourierReady;
    bool guiCourierReady;
    DataStructures::RingBuffer<SWMessage, 32> swbuf;
    DataStructures::RingBuffer<SWMessage, 32> guibuf;

    void init();
    void sendMarklin();
    void sendGUI();
};

#endif
