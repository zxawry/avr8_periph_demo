// file: ssd1306.h
// author: awry

#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

void ssd1306_init(void);
void ssd1306_normal_display(void);
void ssd1306_inverse_display(void);
void ssd1306_draw_block(uint8_t * data);
void ssd1306_draw_ascii(char ascii, uint8_t mask);
void ssd1306_draw_string(char * str, uint8_t mask);
void ssd1306_set_page(uint8_t start, uint8_t end);
void ssd1306_set_column(uint8_t start, uint8_t end);

#endif
