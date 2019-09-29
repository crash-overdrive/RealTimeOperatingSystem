#include "../../include/UART.hpp"
#include "../../include/ts7200.h"

int UART::setConfig(int channel, int wordLength, int fifoEnable, int stop, int parity) {
    int *high, *mid, *low, hmask;
    hmask = 0;
    switch(channel) {
    case COM1:
        high = (int *)(UART1_BASE + UART_LCRH_OFFSET);
        mid = (int *)(UART1_BASE + UART_LCRM_OFFSET);
        low = (int *)(UART1_BASE + UART_LCRL_OFFSET);
        *mid = 0x0;
        *low = 0xBF;
        break;
    case COM2:
        high = (int *)(UART2_BASE + UART_LCRH_OFFSET);
        mid = (int *)(UART2_BASE + UART_LCRM_OFFSET);
        low = (int *)(UART2_BASE + UART_LCRL_OFFSET);
        *mid = 0x0;
        *low = 0x3;
        break;
    default:
        return -1;
        break;
    }
    hmask = wordLength ? hmask | (WLEN_MASK & wordLength<<5) : hmask & ~WLEN_MASK;
    hmask = fifoEnable ? hmask | FEN_MASK : hmask & ~FEN_MASK;
    hmask = stop ? hmask | STP2_MASK : hmask & ~STP2_MASK;
    hmask = parity ? hmask | PEN_MASK : hmask & ~PEN_MASK;
    *high = hmask;
    return 0;
};