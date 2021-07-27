/* Table of buttons' premeasured ADC values and some constants. Also, there are some functions.
 *  Presented ADC values supposed to be used in the 'greater than' evaluations.
 */

#define BUT_DEBOUNCE  10
#define BUT_LONGPRESS 33 //If we are waiting for 2 ticks 15ms each, then 1000ms/(15ms*2)=33
#define BUT_LONG_MASK 0b10000000
#define BUT_NUM_MASK  0b00001111

#define BUT_MUX
#ifdef BUT_MUX
  #define B1 0      //Button 1 is pressed
  #define B2 350    //Button 2 is pressed
  #define B3 470    //Button 3 is pressed
  #define BX 600    //No button pressed
#endif

byte mapButtonNum(word val) {
  if ((val < B2) && (val >= B1))
    return 1;
  if ((val < B3) && (val >= B2))
    return 2;
  if ((val < BX) && (val >= B3))
    return 3;
}

byte packButtonState(byte num, byte longpress) {
  return (num | ((longpress && 1)<<7));
}

void unpackButtonState(byte state, byte* num, byte* longpress){
  *num = state & BUT_NUM_MASK;
  *longpress = state && BUT_LONG_MASK;
}
