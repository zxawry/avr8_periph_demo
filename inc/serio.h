// file: serio.h
// author: awry

#ifndef SERIO_H_
#define SERIO_H_

#include <stdint.h>
#include <avr/pgmspace.h>

int xputc(char c);
int xgetc(void);
int xputs(const char *s);
int xputs_P(PGM_P s);
int xgets(char *b, int n);
int xgets_I(char *b, int n);
void put_dump(uint8_t * b, uint8_t n, uint8_t t);

#endif				// SERIO_H_
