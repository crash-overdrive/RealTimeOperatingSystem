#define VIC1_IRQ_BASE 0x800b0000
#define VIC2_IRQ_BASE 0x800c0000
    #define IRQ_STATUS_OFFSET 0x0
    #define IRQ_SELECT_OFFSET 0xc
    #define IRQ_ENABLE_OFFSET 0x10

#define TC1UI 4 // starting from 0
#define TC2UI 5
#define TC3UI 51 // starting from 32

#define UART1_RX_INTR1 23 // Receive (starting from 23)
#define UART1_TX_INTR1 24 // Transmit
#define UART2_RX_INTR2 25
#define UART2_TX_INTR2 26
#define UART3_RX_INTR3 27
#define UART3_TX_INTR3 28

#define INT_UART1 52
#define INT_UART2 53
#define INT_UART3 54


#define TC1UI_MASK 0x1<<4 // starting from 0
#define TC2UI_MASK 0x1<<5
#define TC3UI_MASK 0x1<<19 // 52-32

#define UART1_RX_INTR1_MASK 0x1<<23 // Receive (starting from 23)
#define UART1_TX_INTR1_MASK 0x1<<24 // Transmit
#define UART2_RX_INTR2_MASK 0x1<<25
#define UART2_TX_INTR2_MASK 0x1<<26
#define UART3_RX_INTR3_MASK 0x1<<27
#define UART3_TX_INTR3_MASK 0x1<<28

#define INT_UART1_MASK 0x1<<20 // 53
#define INT_UART2_MASK 0x1<<22
#define INT_UART3_MASK 0x1<<23
