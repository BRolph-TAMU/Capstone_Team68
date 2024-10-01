#include "functions.h"
#include "globals.h"
#include "pid.h"
#include "twi.h"
#include <util/delay.h>
#define F_CPU = 20000000UL

void inits() {
  TWI_initHost();
  USART0_init();
  PORT_init();
  RTC_init();
  TC_init();
}

void delayms(int ms){
  while(ms){
    _delay_ms(1);
    ms--;
  }
}

char nibbleToHex(uint8_t data){ //turns integer nibble to ascii hex digit
  data &= 0xF; //last 4 bits only
  if (data < 0xA) {return 0x30 + data;} //digits 0x0-9
  else return 0x37 + data; //digits 0xA-F
}

uint8_t hexToNibble(char hex){ //turns ascii hex digit to integer nibble
  if ( (hex < 0x3A) && (hex > 0x2F) ) {return hex - 0x30;} //0x0-9
  else if ( (hex < 0x47) && (hex > 0x40) ) {return hex - 0x37;}//0xA-F
  else if ( (hex < 0x67) && (hex > 0x60) ) {return hex - 0x57;}//0xa-f
  else {return 'H';}
}

void PORT_init() {
  PORTC.DIRSET = (1<<2); //LED
  PORTB.DIRSET = PIN0_bm | PIN1_bm; //twi pins
  PORTB.OUTCLR = PIN0_bm | PIN1_bm;
  PORTC.DIRSET = PIN0_bm | PIN1_bm; //pins for i2c device selection
  PORTC.OUTSET = PIN0_bm | PIN1_bm; //pins high = device disabled
}

uint16_t getCommand(){
  USART0_read();
  uint8_t axis = hexToNibble(USART0_BUFF[1]);
  uint16_t value = hexToNibble(USART0_BUFF[2]<<8 + hexToNibble(USART0_BUFF[3]));
  switch (USART0_BUFF[0]){
    case 'P':
      setProportional(axis, value);
      break;
    case 'I':
      setIntegral(axis, value);
      break;
    case 'D':
      setDerivative(axis, value);
      break;
    case 'R':
      //second char of read command selects sensor
      return readPos(axis);
      break;
    default:
      USART0_sendString("Error\n", 7);
      break;
  }
  return 0;
}

uint16_t readPos(uint8_t sensor){
  if (sensor == PAN){
    PORTC.DIRCLR = PIN0_bm;
    PORTC.DIRSET = PIN1_bm;
    PORTC.OUTSET = PIN1_bm;
  }
  if (sensor == TILT){
    PORTC.DIRCLR = PIN1_bm;
    PORTC.DIRSET = PIN0_bm;
    PORTC.OUTSET = PIN0_bm;
  }
  uint16_t angle = 0;
  TWI_sendAndReadBytes(0x36, 0x0E, TWI_BUFF, TWI_LEN);
  for (uint8_t i = 0; i < TWI_LEN; i++) {
    angle += (TWI_BUFF[i] << 8*(TWI_LEN-1-i));
  }
  return angle;
}

void setProportional(uint8_t axis, uint16_t val){
  val &= 0xFF; //limit to byte only
  if (axis == PAN){panPID.kp = val;}
  if (axis == TILT){tiltPID.kp = val;}
}

void setIntegral(uint8_t axis, uint16_t val){
  val &= 0xFF; //limit to byte only
  if (axis == PAN){panPID.ki = val;}
  if (axis == TILT){tiltPID.ki = val;}
}
void setDerivative(uint8_t axis, uint16_t val){
  val &= 0xFF; //limit to byte only
  if (axis == PAN){panPID.kd = val;}
  if (axis == TILT){tiltPID.kd = val;}
}
