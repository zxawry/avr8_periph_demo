// file: config.h
// author: awry

#ifndef CONFIG_H_
#define CONFIG_H_

// make sure CPU clock frequency is configured.
#ifndef F_CPU
#define F_CPU 8000000UL
#endif // F_CPU

// configure USART baud rate.
#define USART_BAUD 9600

#endif // CONFIG_H_
