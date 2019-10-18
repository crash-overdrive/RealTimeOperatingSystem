#ifndef UART_HPP
#define UART_HPP

class UART {
    public:
        int setConfig(int channel, int wordLength, int fifoEnable, int stop, int parity);
        void enableRXInterrupt(int channel);
        void disableRXInterrupt(int channel);
        void enableTXInterrupt(int channel);
        void disableTXInterrupt(int channel);
        char getc();
};

#endif
