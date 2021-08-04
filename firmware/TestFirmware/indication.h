#pragma once

//ToDo:

#include "lights.h"
#include "pins.h"

#define IND_BLINK_DELAY 500 / portTICK_PERIOD_MS

#define IND_LED_LOW     P_LED_GREEN
#define IND_LED_HIGH    P_LED_BLUE
#define IND_LED_PWR     P_LED_RED


#define IND_S_LOW_PWR   0b01000000    //low power flag
#define IND_S_ADAPTIVE  0b10000000    //adaptive mode flag
#define IND_S_HIGH      0b00000100    //high beam flag
#define IND_S_LOW       0b00000010    //low beam flag
#define IND_S_DAY       0b00010000    //daymode flag

byte indFromLightState(byte light_state) {
  byte ind_state = 0;
  ind_state |= (light_state & (BEAM_LOW_D_MASK | BEAM_REAR_D_MASK))?IND_S_DAY;
}
