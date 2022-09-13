// file: ds1307.h
// author: awry

#ifndef DS1307_H
#define DS1307_H

#include <time.h>

#define DS1307_SQW_MODE_LOW (0x00)
#define DS1307_SQW_MODE_1000HZ (0x10)
#define DS1307_SQW_MODE_4096HZ (0x11)
#define DS1307_SQW_MODE_8192HZ (0x12)
#define DS1307_SQW_MODE_32768HZ (0x13)
#define DS1307_SQW_MODE_HIGH (0x80)

void ds1307_init(void);
void ds1307_set_time(const struct tm *time);
void ds1307_get_time(struct tm *time);

#endif
