#ifndef USART_H
#define USART_H

//definitions
void USART0_init(void);
void USART0_read(void);
void USART0_flush(void);
void USART0_send(char c);
void USART0_sendString(char* str, uint8_t len);

#endif
