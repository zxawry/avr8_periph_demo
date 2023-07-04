// file: ds1307.h
// author: awry

#ifndef DS1307_H
#define DS1307_H

void ds1307_init(void);
void ds1307_set_time(const char *str);
void ds1307_get_time(char *str);

#endif
