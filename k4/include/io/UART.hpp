#ifndef UART_HPP
#define UART_HPP

class UART {
    int base;

public:
    UART(int base);

    int setConfig(int wordLength, int fifoEnable, int stop, int parity);
    int getControlBits();
    void enableRXInterrupt();
    void disableRXInterrupt();
    void enableTXInterrupt();
    void disableTXInterrupt();
    bool isRXEmpty();
    bool isRXFull();
    bool isTXEmpty();
    bool isTXFull();
    char getc();
    void putc(char c);
};

#endif
