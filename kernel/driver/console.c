#include <type.h>
#include <driver.h>
#include <uart.h>
#include <process.h>
#include "../../user/include/file.h"

static u_longlong uartBaseAddr = 0x10010000;

static inline u_int __raw_readl(const volatile void *addr)
{
	u_int val;

	asm volatile("lw %0, 0(%1)" : "=r"(val) : "r"(addr));
	return val;
}

static inline void __raw_writel(u_int val, volatile void *addr)
{
	asm volatile("sw %0, 0(%1)" : : "r"(val), "r"(addr));
}

#define __io_br()	do {} while (0)
#define __io_ar()	__asm__ __volatile__ ("fence i,r" : : : "memory");
#define __io_bw()	__asm__ __volatile__ ("fence w,o" : : : "memory");
#define __io_aw()	do {} while (0)

#define readl(c)	({ u_int __v; __io_br(); __v = __raw_readl(c); __io_ar(); __v; })
#define writel(v,c)	({ __io_bw(); __raw_writel((v),(c)); __io_aw(); })

inline void _putchar(char ch)
{
    int* uartRegTXFIFO = (int*)(uartBaseAddr + UART_REG_TXFIFO);
	while (readl(uartRegTXFIFO) & UART_TXFIFO_FULL);
    writel(ch, uartRegTXFIFO);
}

inline int _getchar(void)
{
    int* uartRegRXFIFO = (int*)(uartBaseAddr + UART_REG_RXFIFO);
	u_int ret = readl(uartRegRXFIFO);
    while (ret & UART_RXFIFO_EMPTY) {
        ret = readl(uartRegRXFIFO);
    }
    if ((ret & UART_RXFIFO_DATA) == '\r')
        return '\n';
    printf("%c", ret & UART_RXFIFO_DATA);
    return ret & UART_RXFIFO_DATA;
}

int consoleWrite(int user_src, u_longlong src, u_longlong start, u_longlong n) {
    int i;
    for (i = 0; i < n; i++) {
         char c;
         if(either_copyin(&c, user_src, src+i, 1) == -1)
             break;
         if (c == '\n')
             _putchar('\r');
         _putchar(c);
    }
    return i;
}

#define GET_BUF_LEN 64
char buf[GET_BUF_LEN];
int consoleRead(int isUser, u_longlong dst, u_longlong start, u_longlong n) {
    int i;
    for (i = 0; i < n; i++) {
        char c = _getchar();
        if (c == '\n')
            _putchar('\r');
         if (either_copyout(isUser, dst + i, &c, 1) == -1)
             break;
    }
    return i;
}

void consoleInit() {
//     devsw[DEV_CONSOLE].read = consoleRead;
//     devsw[DEV_CONSOLE].write = consoleWrite;
}