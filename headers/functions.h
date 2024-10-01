#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <avr/io.h>

void inits(void);
void delayms(int ms);
char nibbleToHex(uint8_t data);
uint8_t hexToNibble(char hex);
void PORT_init(void);
uint16_t getCommand(void);
void setProportional(void);
void setDerivative(void);
void setIntegral(void);
uint16_t readPos(uint8_t sensor);
#endif
