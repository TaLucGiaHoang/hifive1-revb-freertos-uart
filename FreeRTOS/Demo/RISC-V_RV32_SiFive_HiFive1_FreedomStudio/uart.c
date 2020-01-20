/*
 * uart.c
 *
 *  Created on: Jan 14, 2020
 *      Author: HoangSHC
 */

#include <metal/uart.h>
#include <metal/machine.h>

void uart_init(void)
{
	struct metal_uart *uart;
	uart = &__metal_dt_serial_10013000.uart;
	metal_uart_init(uart, 115200);
}

void uart_putc(int c)
{
#define uart0_txdata    (*(volatile uint32_t*)(0x10013000))
#define UART_TXFULL             (1 << 31)
    while ((uart0_txdata & UART_TXFULL) != 0) { }
    uart0_txdata = c;
}

int uart_puts(const char *s)
{
	int len = 0;

	while (*s) {
		uart_putc(*s);
		s++;
		len++;
	}
	return len;
}

int uart_getc(void)
{
#define uart0_rxdata    (*(volatile uint32_t*)(0x10013004))
#define UART_RXEMPTY            (1 << 31)
    uint32_t c = UART_RXEMPTY;
    while (c & UART_RXEMPTY) {
    	c = uart0_rxdata;
    }
    return c & 0xff;
}

