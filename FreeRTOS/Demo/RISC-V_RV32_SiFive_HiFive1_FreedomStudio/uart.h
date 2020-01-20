/*
 * uart.h
 *
 *  Created on: Jan 14, 2020
 *      Author: HoangSHC
 */

#ifndef UART_H_
#define UART_H_

void uart_init(void);
void uart_putc(int c);
int uart_puts(const char *s);
int uart_getc(void);

#endif /* UART_H_ */
