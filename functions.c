#include "functions.h"
#include "globals.h"
#include "pid.h"
#include "twi.h"
#include <util/delay.h>
#include <stdlib.h>
#include <avr/io.h>
#include <math.h>
#define F_CPU = 20000000UL

//init function
void inits() { 
  TWI_initHost(); //init i2c peripheral
  PORT_init(); //gpios
  TC_init(); //timers
}

//delay function
void delayms(uint16_t ms){ 
  while(ms){
    _delay_ms(1); //delay 1 ms
    ms--;
  }
}

//set speed of pan axis
void setPanSpeed(int16_t velocity, uint8_t dir){
  //direction set by velocity sign. boolean dir inverts direction
  if (velocity > 255) {velocity = 255;} //limit checking
  if (velocity < -255) {velocity = -255;}
  //set direction based on sign of velocity. dir variable inverts the logic
  if ((velocity > 0) ^ dir) {PORTA.OUTSET = PANDIR;}
  else {PORTA.OUTCLR = PANDIR;} //turn off motors if speed is 0
  uint8_t speed = abs(velocity); //find motor pwm val
  TCA0.SPLIT.LCMP1 = speed; //set pwm val
}

//set speed of tilt axis, works same as pan axis
void setTiltSpeed(int16_t velocity, uint8_t dir){
  //direction set by velocity sign. boolean dir inverts direction
  if (velocity > 255) {velocity = 255;}
  if (velocity < -255) {velocity = -255;}
  if ((velocity > 0) ^ dir) {PORTA.OUTSET = TILTDIR;}
  else {PORTA.OUTCLR = TILTDIR;}
  uint8_t speed = abs(velocity);
  TCA0.SPLIT.LCMP2 = speed;
}

//this function finds the minimum speed required to move the axis
//doing this reduces hystersis on the motion
//which makes the pid loop run better
int16_t findMinSpeed(uint8_t axis){
  int16_t currentpos = (int16_t)readPos(axis); //read axis pos
  int16_t velocity = 0; //set speed to 0
  //increase motor speed until the axis moves
  while (abs((int16_t)readPos(axis)-currentpos) < 10) { //axis moved?
    if (axis == PAN){setPanSpeed(velocity, 0);} //set speed
    if (axis == TILT){setTiltSpeed(velocity, 0);}
    velocity += 1; //increase speed for next loop
    delayms(15);
  }
  setPanSpeed(0,0); //turn off motors
  setTiltSpeed(0,0);
  return velocity;
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

uint8_t intToHexChar(char* buffer, int32_t input){
  uint8_t index = 8;
  uint8_t temp = 0;
  if (buffer != NULL){
    buffer[8] = '\n';
    while (index != 0) {
      index--;
      buffer[index] = nibbleToHex(input & 0xF);
      input = input >> 4;
    }
  }
  return 0;
}
  

void PORT_init() {
  PORTA.DIRSET = LEDPIN; //LED
  PORTB.DIRSET = PIN0_bm | PIN1_bm; //twi pins
  PORTB.OUTCLR = PIN0_bm | PIN1_bm;
  PORTC.DIRSET = PIN0_bm | PIN1_bm; //pins for i2c device selection
  PORTC.OUTSET = PIN0_bm | PIN1_bm; //pins high = device disabled
  PORTA.DIRSET = PANDIR | TILTDIR; //motor direction pins
  PORTB.DIRSET = PANPWM | TILTPWM; //motor speed pins
}

uint16_t getCommand(){
  USART0_read();
  uint8_t axis = hexToNibble(USART0_BUFF[1]); //second char selects axis
  //convert hex chars to integer
  uint16_t value = ( ((uint16_t)hexToNibble(USART0_BUFF[2])<<8)
		  +  ((uint16_t)hexToNibble(USART0_BUFF[3])<<4)
		  +  (uint16_t)hexToNibble(USART0_BUFF[4])    );
  char ident = USART0_BUFF[0]; //command identifier character
  USART0_send(ident); //repeat back command character
  USART0_send('\n');
  switch (USART0_BUFF[0]){
    case 'P':
      setProportional(axis, value>>4);
      break;
    case 'I':
      setIntegral(axis, value>>4);
      break;
    case 'D':
      setDerivative(axis, value>>4);
      break;
    case 'R':
      //second char of read command selects sensor
      return readPos(axis);
      break;
    case 'O':
      setPoint(axis, value);
      break;
    case 'F':
      printParams(axis);
      break;
    case 'L':
      PORTA.OUTTGL = LEDPIN;
      break;
    default:
      USART0_sendString("Error\n", 7);
      break;
  }
  return 0;
}

uint16_t readPos(uint8_t sensor){
  if (sensor == PAN){
    PORTC.DIRCLR = PANEN;
    PORTC.DIRSET = TILTEN;
    PORTC.OUTSET = TILTEN;
  }
  if (sensor == TILT){
    PORTC.DIRCLR = TILTEN;
    PORTC.DIRSET = PANEN;
    PORTC.OUTSET = PANEN;
  }
  uint16_t angle = 0;
  TWI_sendAndReadBytes(0x36, 0x0E, TWI_BUFF, TWI_LEN);
  for (uint8_t i = 0; i < TWI_LEN; i++) {
    angle += (TWI_BUFF[i] << 8*(TWI_LEN-1-i));
  }
  return angle;
}


