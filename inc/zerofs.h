// file: zerofs.h
// author: awry

#ifndef ZEROFS_H
#define ZEROFS_H

#include <stdint.h>

uint8_t zerofs_init(void);
uint8_t zerofs_mount(void);
uint8_t zerofs_create(void);
uint8_t zerofs_read(char * data, uint32_t index, uint8_t count);
//uint8_t zerofs_read(char * data, uint32_t sector, uint16_t offset, uint8_t count);
uint8_t zerofs_write(char * data, uint32_t index, uint8_t count);
//uint8_t zerofs_write(char * data, uint32_t sector, uint16_t offset, uint8_t count);
uint8_t zerofs_print(uint32_t sector);

#endif
