#ifndef UART_HPP
#define UART_HPP

#define BPF8	3
#define BPF7	2
#define BPF6	1
#define BPF5	0

class UART {
    public:
        int setConfig(int channel, int wordLength, int fifoEnable, int stop, int parity);
};

#endif
