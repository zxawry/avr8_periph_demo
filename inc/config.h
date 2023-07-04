// file: config.h
// author: awry

#ifndef CONFIG_H_
#define CONFIG_H_

// make sure CPU clock frequency is configured.
#ifndef F_CPU
#define F_CPU (11059200UL)
#endif				// F_CPU

// configure USART baud rate.
#define USART_BAUD (9600U)

// configure TWI clock frequency.
#define TWI_FREQ (100000UL)

// configure size of the queue.
#define QUEUE_SIZE (64U)

#endif				// CONFIG_H_
