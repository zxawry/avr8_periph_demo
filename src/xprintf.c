/*------------------------------------------------------------------------/
/  Universal String Handler for Console Input and Output
/-------------------------------------------------------------------------/
/
/ Copyright (C) 2021, ChaN, all right reserved.
/
/ xprintf module is an open source software. Redistribution and use of
/ xprintf module in source and binary forms, with or without modification,
/ are permitted provided that the following condition is met:
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/-------------------------------------------------------------------------*/

#include "xprintf.h"

#if XF_USE_OUTPUT
#include <stdarg.h>
int (*xfunc_output)(char);	/* Pointer to the default output device */

/*----------------------------------------------*/
/* Put a character                              */
/*----------------------------------------------*/

void xputc (
	char chr				/* Character to be output */
)
{
	if (chr == '\a') return;

	if (chr == '\n') xputc('\r');	/* CR -> CRLF */

	while(xfunc_output(chr) < 0);	/* Output it to the default output device */
}

/*----------------------------------------------*/
/* Put a null-terminated string                 */
/*----------------------------------------------*/

void xputs (			/* Put a string to the default device */
	const char* str		/* Pointer to the string */
)
{
	while (*str) {			/* Put the string */
		xputc(*str++);
	}
}

#endif	/* XF_USE_OUTPUT */

#if XF_USE_INPUT
int (*xfunc_input)(void);	/* Pointer to the default input stream */

/*----------------------------------------------*/
/* Get a character from the input                    */
/*----------------------------------------------*/

char xgetc (			/* 0:End of stream, 1:A line arrived */
	void
)
{
	int c;

	do {
		c = xfunc_input();		/* Get a char from the incoming stream */
	} while(c < 0);

	if (c == '\r') c = '\n';	/* Simulate unix-like terminal */
	if (c == '\n') xputc(c);	/* End of line? */
	return (char) c;
}

/*----------------------------------------------*/
/* Get a line from the input                    */
/*----------------------------------------------*/

int xgets (			/* 0:End of stream, 1:A line arrived */
	char* buff,		/* Pointer to the buffer */
	int len			/* Buffer length */
)
{
	int c, i;

	i = 0;
	for (;;) {
		c = xgetc();			/* Get a char from the incoming stream */
		if (c < 0) break;		/* End of stream? */
		if (c == '\r') c = '\n';	/* Simulate unix-like terminal */
		if (c == '\n') {		/* End of line? */
			buff[i++] = c;
			break;
		}
		//if ((c == '\b' || c == '\x7f') && i > 0) {		/* BS? */
		if (c == '\x7f' && i > 0) {		/* BS? */
			i--;
			xputc('\b');
			xputc(' ');
			xputc('\b');
			continue;
		}
		if (((c >= ' ' && c <= '\x7e') || c >= '\xa0') && i < len - 1) {	/* Visible chars? */
			buff[i++] = c;
			xputc(c);
		}
	}
	buff[i] = 0;	/* Terminate with a \0 */
	return (int)(c == '\n');
}

#endif /* XF_USE_INPUT */
