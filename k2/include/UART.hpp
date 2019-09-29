#ifndef UART_HPP
#define UART_HPP

class UART {
    public:
        int setConfig(int channel, int wordLength, int fifoEnable, int stop, int parity);
};

#endif
