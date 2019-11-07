#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/client/Test.hpp"
#include "user/message/TextMessage.hpp"
#include "user/syscall/UserSyscall.hpp"
#define FOREVER for(;;)

void testPrinting() {

    // int UART1_SERVER = WhoIs("UART1RX");
    // int UART2_RX_SERVER = WhoIs("UART2RX");
    int UART2_TX_SERVER = WhoIs("UART2TX");
    // bwprintf(COM1, "u.%d!", UART2_TX_SERVER);
    for (int i = 0; i < 100000; ++i) {
        int x = Putc(UART2_TX_SERVER, UART2, 'S');
        // bwprintf(COM1, "x");
        if (x < 0) {
            bwprintf(COM1, "test: %d", x);
        }
        // bwprintf(COM1, "%d", x);
    }
    Putc(UART2_TX_SERVER, UART2, 'd');
    // bwprintf(COM2, "\r\n\r\n###Fake loop ended###\r\n\r\n");

        // int ch = Getc(UART2_RX_SERVER, UART2);
        // if (ch < 0) {
        //     bwprintf(COM2, "Getc didn't succeed!");
        // }
        // for (int i = 0; i <= 100; ++i) {
        //     // bwprintf(COM1, "u.%d!", UART2_TX_SERVER);
        //     y = Putc(UART2_TX_SERVER, UART2, 'S');
        //     // int x = Putc(UART2_TX_SERVER, UART2, ch);
        //     if (y < 0) {
        //         bwprintf(COM2, "Putc didn't succeed!");
        //     }
        // }
        // bwprintf(COM2, "\r\n\r\n###Loop ended###\r\n\r\n");

    Exit();

}

void testTyping() {
    int UART2_RX_SERVER = WhoIs("UART2RX");
    int UART2_TX_SERVER = WhoIs("UART2TX");
    // bwprintf(COM1, "u.%d!", UART2_TX_SERVER);
    FOREVER {
        int x = Getc(UART2_RX_SERVER, UART2);
        for (int i = 0; i < 100; ++i) {
            Putc(UART2_TX_SERVER, UART2, x);
        }
    }
    Putc(UART2_TX_SERVER, UART2, 'e');
    Exit();
}

void testTCStr() {
    TextMessage textmsg;
    char msg[9] = "tr 58 14";
    for (int i = 0; i < 9; ++i) {
        textmsg.msg[i] = msg[i];
    }
    textmsg.msglen = 9;

    Send(8, (char*)&textmsg, textmsg.size(), "reply", 6);

    Exit();
}

void testTCSsw() {
    TextMessage textmsg;
    char msg[7] = "sw 7 C";
    for (int i = 0; i < 7; ++i) {
        textmsg.msg[i] = msg[i];
    }
    textmsg.msglen = 7;

    Send(8, (char*)&textmsg, textmsg.size(), "reply", 6);

    Exit();
}
