#include "functions.h"

#define NORM_RANGE_L 0   //Minimum value after normalization
#define NORM_RANGE_R 15  //Maximum value after normalization

#define LUX_MIN 710      //Measured in a shade of a palm
#define LUX_MAX 5        //Measured in a direct light from a lamp
#define LUX_MASK 0b11110000

#define VOLTAGE_MIN 3    // Don't know yet
#define VOLTAGE_MAX 300  // Don't know yet
#define VOLTAGE_MASK 0b00001111

byte getNormLux(word raw_lux) {
  return mmNormalization(NORM_RANGE_L, NORM_RANGE_R,
                         raw_lux, LUX_MAX, LUX_MIN);
}

byte getNormVoltage(word raw_volt) {
  return mmNormalization(NORM_RANGE_L, NORM_RANGE_R,
                         raw_volt, VOLTAGE_MAX, VOLTAGE_MIN);
  
}

byte packNormLuxVoltage(byte norm_lux, byte norm_volt) {
  //Pack two 4-bits values into one byte, lux on the left half
  return (norm_lux << 4) | norm_volt;
}

void unpackNormLuxVoltage(byte pack, byte *norm_lux, byte *norm_volt) {
  *norm_lux = pack >> 4;
  *norm_volt = pack & VOLTAGE_MASK;
}
