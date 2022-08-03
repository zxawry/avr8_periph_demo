// file: usart.h
// author: awry

#ifndef USART_H_
#define USART_H_

// initiate USART peripheral.
void usart_init(void);

// send one byte of data down the USART TX line.
int usart_putc(char data);

// receive one byte of data from the USART RX line.
int usart_getc(void);

#endif				// USART_H_
