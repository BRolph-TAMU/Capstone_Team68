#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <avr/io.h>

void inits(void);
void delayms(int ms);
char nibbleTo(uint8_t data);
uint8_t hexToNibble(char hex);
void PORT_init(void);
#endif
