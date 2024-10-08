#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <avr/io.h>

void inits(void);
void delayms(uint16_t ms);
char nibbleToHex(uint8_t data);
uint8_t hexToNibble(char hex);
void PORT_init(void);
uint16_t getCommand(void);
void motorSet(int16_t velocity, uint8_t dir);
uint16_t readPos(uint8_t sensor);
uint8_t intToHexChar(char* buffer, int32_t input);
#endif
