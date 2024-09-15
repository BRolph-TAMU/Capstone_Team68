#ifndef GLOBALS_H
#define GLOBALS_H

//definitions
#include <avr/io.h>
#define USART_LEN 8
#define TWI_LEN 2
#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

char USART0_BUFF[USART_LEN];
uint8_t TWI_BUFF[TWI_LEN];

#endif
