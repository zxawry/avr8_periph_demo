/*------------------------------------------------------------------------*/
/* Universal string handler for user console interface  (C)ChaN, 2021     */
/*------------------------------------------------------------------------*/

#ifndef XPRINTF_DEF
#define XPRINTF_DEF
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XF_USE_OUTPUT   1       /* 1: Enable output functions */
#define XF_USE_INPUT    1       /* 1: Enable input functions */

#if defined(__GNUC__) && __GNUC__ >= 10
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

#if XF_USE_OUTPUT
#define xdev_out(func) xfunc_output = (int(*)(char))(func)
extern int (*xfunc_output)(char);
void xputc (char chr);
void xputs (const char* str);
#endif

#if XF_USE_INPUT
#define xdev_in(func) xfunc_input = (int(*)(void))(func)
extern int (*xfunc_input)(void);
char xgetc (void);
int xgets (char* buff, int len);
#endif

#ifdef __cplusplus
}
#endif

#endif
