#ifndef ROCKLING_RGB_H_
#define ROCKLING_RGB_H_

#include <stdint.h>

void rgb_init(void);
void rgb_set(uint8_t r, uint8_t g, uint8_t b);
void rgb_on_time(uint8_t ms);
void rgb_off_time(uint8_t ms);
void rgb_in_time(uint8_t ms);
void rgb_out_time(uint8_t ms);

#endif
