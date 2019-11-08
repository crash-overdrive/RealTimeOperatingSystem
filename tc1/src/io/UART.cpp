#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "io/UART.hpp"

UART::UART(int base) : base{base} {}

int UART::setConfig(int wordLength, int fifoEnable, int stop, int parity) {
    int *high, *mid, *low, hmask;
    hmask = 0;
    high = (int *)(base + UART_LCRH_OFFSET);
    mid = (int *)(base + UART_LCRM_OFFSET);
    low = (int *)(base + UART_LCRL_OFFSET);
    switch(base) {
    case UART1_BASE:
        *mid = 0x0;
        *low = 0xBF;
        break;
    case UART2_BASE:
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

int UART::getControlBits() {
    volatile int flags;
    flags = *(int *)(base + UART_CTRL_OFFSET);
    return flags;
}

void UART::enableRXInterrupt() {
    volatile int flags;
    flags = *(int *)(base + UART_CTRL_OFFSET);
    *(int *)(base + UART_CTRL_OFFSET) = flags | RIEN_MASK;
}

void UART::disableRXInterrupt() {
    volatile int flags;
    flags = *(int *)(base + UART_CTRL_OFFSET);
    *(int *)(base + UART_CTRL_OFFSET) = flags & ~RIEN_MASK;
}

void UART::enableTXInterrupt() {
    volatile int flags;
    flags = *(int *)(base + UART_CTRL_OFFSET);
    *(int *)(base + UART_CTRL_OFFSET) = flags | TIEN_MASK;
}

void UART::disableTXInterrupt() {
    volatile int flags;
    flags = *(int *)(base + UART_CTRL_OFFSET);
    *(int *)(base + UART_CTRL_OFFSET) = flags & ~TIEN_MASK;
}

bool UART::isMISInterrupt() {
    volatile bool m = *(int *)(base + UART_INTR_OFFSET) & MIS_MASK;
    return m;
}

bool UART::isRXInterrupt() {
    volatile bool rx = *(int *)(base + UART_INTR_OFFSET) & RIS_MASK;
    return rx;
}

void UART::clearRXInterrupt() {
    volatile int flags;
    flags = *(int *)(base + UART_INTR_OFFSET);
    *(int *)(base + UART_INTR_OFFSET) = flags & ~RIS_MASK;
}

bool UART::isTXInterrupt() {
    volatile bool tx = *(int *)(base + UART_INTR_OFFSET) & TIS_MASK;
    return tx;
}

void UART::clearTXInterrupt() {
    volatile int flags;
    flags = *(int *)(base + UART_INTR_OFFSET);
    *(int *)(base + UART_INTR_OFFSET) = flags & ~TIS_MASK;
}

bool UART::isRXEmpty() {
    volatile bool empty = *(int *)(base + UART_FLAG_OFFSET) & RXFE_MASK;
    return empty;
}

bool UART::isRXFull() {
    volatile bool full = *(int *)(base + UART_FLAG_OFFSET) & RXFF_MASK;
    return full;
}

bool UART::isTXEmpty() {
    volatile bool empty = *(int *)(base + UART_FLAG_OFFSET) & TXFE_MASK;
    return empty;
}

bool UART::isTXFull() {
    volatile bool full = *(int *)(base + UART_FLAG_OFFSET) & TXFF_MASK;
    return full;
}

bool UART::isClearToSend() {
    volatile bool cts = *(int *)(base + UART_FLAG_OFFSET) & CTS_MASK;
    return true;
}

char UART::getc() {
    volatile char c;
    c = (char)(*(volatile int *)(base + UART_DATA_OFFSET) & DATA_MASK);
    return c;
}

void UART::putc(char c) {
    *(volatile int *)(base + UART_DATA_OFFSET) = c;
}
