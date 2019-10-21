#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/client/Test.hpp"
#include "user/syscall/UserSyscall.hpp"
#define FOREVER for(;;)

void testTyping() {
    
    int UART1_SERVER = WhoIs("UART1RX");
    int UART2_RX_SERVER = WhoIs("UART2RX");
    int UART2_TX_SERVER = WhoIs("UART2TX");
    // bwprintf(COM1, "u.%d!", UART2_TX_SERVER);
    for (int i = 0; i < 100; ++i) {
        int x = Putc(UART2_TX_SERVER, UART2, 'S');
        // bwprintf(COM1, "x");
        if (x < 0) {
            bwprintf(COM1, "%d", x);
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