/* Table of buttons' premeasured ADC values and some constants. Also, there are some functions.
 *  Presented ADC values supposed to be used in the 'greater than' evaluations.
 */

#pragma once

#define BUT_DEBOUNCE            10
#define BUT_LONGPRESS           33    //If we are waiting for 2 ticks 15ms each, then 1000ms/(15ms*2)=33
#define BUT_LONG_MASK   0b10000000
#define BUT_NUM_MASK    0b00001111

#define BUT_MUX                       //comment out if there are no multiplexed buttons

#ifdef BUT_MUX
  #define B1                     0    //Button 1 is pressed
  #define B2                   500    //Button 2 is pressed
  #define B3                   650    //Button 3 is pressed
  #define BX                   700    //No button is pressed
#endif

// *** Digital buttons numbers ***

//a thingie for a far future, I'll not touch it
#define BUT_A                    1
#define BUT_B                    2
#define BUT_C                    3

/* *** Button codes: ***
 * Short    Long
 * 1        129
 * 2        130
 * 3        131
 */
#define BUT_1S                   1
#define BUT_1L                 129
#define BUT_2S                   2
#define BUT_2L                 130
#define BUT_3S                   3
#define BUT_3L                 131
 
#define ACT_FLASH           BUT_2S
#define ACT_HIGH_BEAM       BUT_2L
#define ACT_POWER           BUT_3L
#define ACT_ADAPTIVE        BUT_1S
#define ACT_LOW_BEAM        BUT_3S


// *** Functions section ***

byte mapButtonNum(word val) {    //Returns the button number by entering the ADC value
  if ((val < B2) && (val >= B1))
    return BUT_A;
  if ((val < B3) && (val >= B2))
    return BUT_B;
  if ((val < BX) && (val >= B3))
    return BUT_C;
}

byte packButtonState(byte num, byte longpress) {   //Adds longpress flag to the left half of a byte
  return (num | ((longpress && 1)<<7));
}

void unpackButtonState(byte state, byte* num, byte* longpress){  //Unpacks one state byte into the button number and the longetivity flag
  *num = state & BUT_NUM_MASK;
  *longpress = state && BUT_LONG_MASK;
}
