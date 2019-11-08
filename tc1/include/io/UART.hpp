#ifndef UART_HPP
#define UART_HPP

class UART {
    int base;
    int rxIRQEnabled = false;
    int txIRQEnabled = false;

public:
    UART(int base);

    int setConfig(int wordLength, int fifoEnable, int stop, int parity);
    int getControlBits(); // TODO: remove this later here and in .cpp
    void enableRXInterrupt();
    void disableRXInterrupt();
    void enableTXInterrupt();
    void disableTXInterrupt();
    bool isMISInterrupt();
    bool isRXInterrupt();
    void clearRXInterrupt();
    bool isTXInterrupt();
    void clearTXInterrupt();
    bool isRXEmpty();
    bool isRXFull();
    bool isTXEmpty();
    bool isTXFull();
    bool isClearToSend();
    char getc();
    void putc(char c);
};

#endif
