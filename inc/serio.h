// file: serio.h
// author: awry

#ifndef SERIO_H_
#define SERIO_H_

int xputc(char c);
int xgetc(void);
int xputs(const char *s);
int xgets(char *b, int n);
void put_dump(const char *input);

#endif				// SERIO_H_
