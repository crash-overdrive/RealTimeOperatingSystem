#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/client/Test.hpp"
#include "user/syscall/UserSyscall.hpp"
#define FOREVER for(;;)

void testTyping() {
    
    const int UART1_SERVER = WhoIs("UART1RX");
    const int UART2_RX_SERVER = WhoIs("UART2RX");
    const int UART2_TX_SERVER = WhoIs("UART2TX");

    FOREVER {
        int ch = Getc(UART2_RX_SERVER, UART2);
        if (ch < 0) {
            // bwprintf(COM2, "BAD!!");
        }
        for (int i = 0; i <= 100; ++i) {
            int x = Putc(UART2_TX_SERVER, UART2, ch);
            if (x < 0) {
                // bwprintf(COM2, "BAD2!!");
            }
        }
        
    }
}