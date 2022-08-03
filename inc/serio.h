// file: serio.h
// author: awry

#ifndef SERIO_H_
#define SERIO_H_

#include <stdint.h>

int xputc(char c);
int xgetc(void);
int xputs(const char *s);
int xgets(char *b, int n);
void put_dump(uint8_t * b, uint8_t n);

#endif				// SERIO_H_
