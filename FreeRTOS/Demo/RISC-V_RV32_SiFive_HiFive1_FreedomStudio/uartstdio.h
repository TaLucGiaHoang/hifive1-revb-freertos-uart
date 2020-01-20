/*
 * uartstdio.h
 *
 *  Created on: Jan 14, 2020
 *      Author: HoangSHC
 */

#ifndef UARTSTDIO_H_
#define UARTSTDIO_H_

int uart_gets(char *pcBuf, uint32_t ui32Len);
int uart_write(const char *pcBuf, uint32_t ui32Len);
void uart_vprintf(const char *pcString, va_list vaArgP);
void uart_printf(const char *pcString, ...);


#endif /* UARTSTDIO_H_ */
