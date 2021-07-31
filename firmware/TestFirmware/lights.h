#pragma once

#include "pins.h"

// *** PWM values section ***

#define BEAM_LOW_VAL    128
#define BEAM_HIGH_VAL   255
#define BEAM_REAR_VAL   128
#define BEAM_DAY_VAL     64    //This value is applied to rear and low beams, high is off

#define BEAM_HIGH_THRES  64    //Minimum value for reversing flash action from "old_val-255-old_val" to "old_val-0-old_val"

// *** Special features section

#define BEAM_HYBRID     true   //If high and low beam is the same


// *** Helper definitions ***

#define BEAM_LOW_NUM    0
#define BEAM_HIGH_NUM   1
#define BEAM_REAR_NUM   2
#define BEAM_DAY_NUM    3        //virtual

#define BEAM_LOW    P_BEAM_LOW   //helper, alias of pin
#define BEAM_REAR   P_BEAM_REAR  //helper, alias of pin
#define BEAM_DAY    128          //virtual pin
#ifdef BEAM_HYBRID
  #define BEAM_HIGH P_BEAM_LOW
#elif
  #define BEAM_HIGH P_BEAM_HIGH
#endif
  
#define BEAM_SOFT_START true   //Slowly rise PWM value

#ifdef BEAM_SOFT_START
  #define BEAM_SS_DELAY   100 / portTICK_PERIOD_MS  //Rising PWM delay, ms
  #define BEAM_SS_STEP     20                       //PWM increment
#endif

// *** Adaptive mode section ***

/* The value from a photoresistor is normalized and packed into 4 bits, 
 *  so there are only 16 (0..15) levels of luminosity.
 *  Insert values that define, which beam (or power applied to one)
 *  should be selected automatically.
 *  ToDo:
 */
#define BEAM_AD_DAY_THRES    0
#define BEAM_AD_LOW_THRES    0
#define BEAM_AD_HIGH_THRES   0

#define BEAM_AD_HYST         2000 / portTICK_PERIOD_MS //Hysteresis time, prevents strobing, while riding
                                                       // from shadows to lights multiple times

// *** States section ***

/* Here we have one byte bitmap with flags, representing binary states.
 *     TRANS  |  BEAM
 *  [ 0 0 0 0 | 0 0 0 0 ]
 *    a h l r | D H L R
 *  D - daytime lights
 *  H - high beam, if set - turned on according to the table of values
 *  L - low beam, if set - turned on according to the table of values
 *  R - rear lights, if set - turned on according to the table of values
 *  a - set if adaptive mode is on
 *  h - set if high beam in transition
 *  l - set if low beam in transition
 *  r - set if rear beam in transition
 */

//Power state-half flags
#define BEAM_DAY_MASK       0b00001000
#define BEAM_HIGH_MASK      0b00000100
#define BEAM_LOW_MASK       0b00000010
#define BEAM_REAR_MASK      0b00000001

//Transition-half flags, not in use yet
#define BEAM_ADAPT_MASK     0b10000000
#define BEAM_HIGH_T_MASK    0b01000000
#define BEAM_LOW_T_MASK     0b00100000
#define BEAM_REAR_T_MASK    0b00010000

// *** Functions section ***

#define DELAY(t) vTaskDelay(t)

byte mapBeamMask(byte beam){  //function returns mask by entering the beam pin
  switch(beam){
    case BEAM_LOW:
      return BEAM_LOW_MASK;
      
    #ifndef BEAM_HYBRID
    case BEAM_HIGH:        
      return BEAM_HIGH_MASK;
    #endif
    
    case BEAM_REAR:
      return BEAM_REAR_MASK;
    case BEAM_DAY:
      return BEAM_DAY_MASK;
  }
}

byte mapBeamVal(byte beam){  //function returns const PWM value by entering the beam pin
  switch(beam){
    case BEAM_LOW:
      return BEAM_LOW_VAL;

    #ifndef BEAM_HYBRID
    case BEAM_HIGH:       
      return BEAM_HIGH_VAL;
    #endif
    
    case BEAM_REAR:
      return BEAM_REAR_VAL;
    case BEAM_DAY:
      return BEAM_DAY_VAL;
  }
}

byte mapBeamNum(byte beam){  //function returns the number of beam. Used for taking a value from an array with beams' pwms
  switch(beam){
    case BEAM_LOW:
      return BEAM_LOW_NUM;

    #ifndef BEAM_HYBRID
    case BEAM_HIGH:        
      return BEAM_HIGH_NUM;
    #endif
    
    case BEAM_REAR:
      return BEAM_REAR_NUM;
    case BEAM_DAY:
      return BEAM_DAY_NUM;
  }
}

void beamSoftStart(byte beam, byte fin_val){  //SoftStart function, fade in the light
  for (byte val = BEAM_SS_STEP; val <= fin_val; val += BEAM_SS_STEP){
     analogWrite(beam, val);
     DELAY(BEAM_SS_DELAY);
  }
}

void beamSoftStop(byte beam, byte fin_val, byte cur_val){  //SoftStop function, fade out the light
  for (; cur_val >= fin_val; cur_val -= BEAM_SS_STEP){
     analogWrite(beam, cur_val);
     DELAY(BEAM_SS_DELAY);
  }
}

void beamSwitch(byte beam, byte state, byte* cur_val_arr){      //Turn on or off the beam
  byte mask = mapBeamMask(beam);
  byte set_val = mapBeamVal(beam);
  byte num = mapBeamNum(beam);
  
  if (BEAM_SOFT_START) {            //if softstart enabled
    if (state & mask)               //if beam turned on
      beamSoftStop(beam, 0, cur_val_arr[num]);  //ToDo: hybrid mode value for high beam
    else
      beamSoftStart(beam, set_val);
  }
  else {                           //if softstart disabled
    if (state & mask)  //if beam turned on
      analogWrite(beam, 0);       //ToDo: hybrid mode value for high beam
    else
      analogWrite(beam, set_val);
  }
}
