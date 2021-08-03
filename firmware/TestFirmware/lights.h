#pragma once

#include "pins.h"

// *** PWM values section ***

#define BEAM_LOW_VAL    128
#define BEAM_HIGH_VAL   255
#define BEAM_REAR_VAL   128
#define BEAM_DAY_VAL     32    //This value is applied to rear and low beams, high is off

#define BEAM_HIGH_THRES  64    //Minimum value for reversing flash action from "old_val-255-old_val" to "old_val-0-old_val"

// *** Special features section

#define BEAM_HYBRID     true   //If high and low beams are same


// *** Helper definitions ***

#define BEAM_LOW          0
#define BEAM_HIGH         1
#define BEAM_REAR         2
#define BEAM_DAY          3        //virtual

#define P_BEAM_DAY      128        //virtual pin, maybe it should be placed in "pins.h"?

//#define BEAM_LOW_PIN    P_BEAM_LOW   //helper, alias of pin
//#define BEAM_REAR_PIN   P_BEAM_REAR  //helper, alias of pin
//#define BEAM_DAY_PIN    128          //virtual pin
//#ifdef BEAM_HYBRID
//  #define BEAM_HIGH P_BEAM_LOW
//#elif
//  #define BEAM_HIGH P_BEAM_HIGH
//#endif
  
#define BEAM_SOFT_START false   //Slowly rise PWM value

#ifdef BEAM_SOFT_START
  #define BEAM_SS_DELAY   100 / portTICK_PERIOD_MS  //Rising PWM delay, ms
  #define BEAM_SS_STEP     5                        //PWM increment
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
 *     MODE   |  BEAM
 *  [ 0 0 0 0 | 0 0 0 0 ]
 *    a - l r | - H L R
 *  
 *  H - high beam, if set - turned on according to the table of values
 *  L - low beam, if set - turned on according to the table of values
 *  R - rear lights, if set - turned on according to the table of values
 *  a - set if adaptive mode is on
 *  l - set if low beam in a day mode
 *  r - set if rear beam in a day mode
 */

//Power state-half flags
#define BEAM_HIGH_MASK      0b00000100
#define BEAM_LOW_MASK       0b00000010
#define BEAM_REAR_MASK      0b00000001

//Mode-half flags, not in use yet
#define BEAM_ADAPT_MASK     0b10000000
#define BEAM_LOW_D_MASK     0b00100000
#define BEAM_REAR_D_MASK    0b00010000

// *** Functions section ***
#ifndef DELAY(t)
  #define DELAY(t) vTaskDelay(t)
#endif

byte mapBeamMask(byte beam){  //function returns mask by entering the beam num
  switch(beam){
    case BEAM_LOW:
      return BEAM_LOW_MASK;
      
    case BEAM_HIGH:
      if (BEAM_HYBRID)        
        return BEAM_LOW_MASK;
      else
        return BEAM_HIGH_MASK;
        
    case BEAM_REAR:
      return BEAM_REAR_MASK;
      
    case BEAM_DAY:
      return 0b00000000;        //What is this? I don't know!
  }
}

byte mapBeamDayMask(byte beam){  //function returns day mask by entering the beam num
  switch(beam){
    case BEAM_LOW:
      return BEAM_LOW_D_MASK;
        
    case BEAM_REAR:
      return BEAM_REAR_D_MASK;

    case BEAM_HIGH:
      return BEAM_LOW_D_MASK;
      
    case BEAM_DAY:
      return 0b00000000;        //What is this? I don't know! [1]

    default:
      return 0b00000000;
  }
}

byte mapBeamVal(byte beam){  //function returns const PWM value by entering the beam num
  switch(beam){
    case BEAM_LOW:
      return BEAM_LOW_VAL;

    case BEAM_HIGH:
      if (BEAM_HYBRID)        
        return BEAM_LOW_VAL;
      else
        return BEAM_HIGH_VAL;
    
    case BEAM_REAR:
      return BEAM_REAR_VAL;
      
    case BEAM_DAY:
      return 0b00000000;        //What is this? I don't know! [2]
  }
}

byte mapBeamPin(byte beam){  //function returns a pin of the beam
  switch(beam){
    case BEAM_LOW:
      return P_BEAM_LOW;

    case BEAM_HIGH:
      if (BEAM_HYBRID)        
        return P_BEAM_LOW;
      else
        return P_BEAM_HIGH;
    
    case BEAM_REAR:
      return P_BEAM_REAR;
      
    case BEAM_DAY:
      return P_BEAM_DAY;        //What is this? It's the virtual pin, but why..? I don't know!
  }
}

void beamSoftStart(byte pin, byte fin_val, byte* cur_val){  //SoftStart function, fade in the light
  if (!BEAM_HYBRID)
    *cur_val = BEAM_SS_STEP;
    
  for (; (fin_val - *cur_val >= BEAM_SS_STEP); *cur_val += BEAM_SS_STEP){  //We don't want to reset 1 byte and go in an infinite loop
     analogWrite(pin, *cur_val);
     DELAY(BEAM_SS_DELAY);
  }
  *cur_val += fin_val - *cur_val;     //Adding the residue
  analogWrite(pin, *cur_val);
}

void beamSoftStop(byte pin, byte fin_val, byte* cur_val){  //SoftStop function, fade out the light
  for (; (*cur_val - fin_val >= BEAM_SS_STEP); *cur_val -= BEAM_SS_STEP){  //We don't want to reset 1 byte and go in an infinite loop
     analogWrite(pin, *cur_val);
     DELAY(BEAM_SS_DELAY);
  }
  *cur_val -= *cur_val - fin_val;    //Substracting the residue
  analogWrite(pin, *cur_val);
}

void beamStart(byte pin, byte fin_val, byte* cur_val) {    //A meta-function, which redirects (or not) the execution to the soft-version
  if (BEAM_SOFT_START)
    beamSoftStart(pin, fin_val, cur_val);
  else {
    analogWrite(pin, fin_val);
    *cur_val = fin_val;
  }
}

void beamStop(byte pin, byte fin_val, byte* cur_val) {     //A meta-function, which redirects (or not) the execution to the soft-version
  if (BEAM_SOFT_START)
    beamSoftStop(pin, fin_val, cur_val);
  else {
    analogWrite(pin, fin_val);
    *cur_val = fin_val;
  }
}

void beamSwitch(byte beam, byte* state, byte* cur_val_arr){      //Turn on or off the beam
  byte mask = mapBeamMask(beam);
  byte mask_d = mapBeamDayMask(beam);
  byte set_val = mapBeamVal(beam);
  byte pin = mapBeamPin(beam);
  
    //**********
    if (*state & mask) {                                //if beam turned on and not in a daylight mode
      if (beam == BEAM_HIGH)
        if (BEAM_HYBRID) {
          beamStop(P_BEAM_LOW, BEAM_LOW_VAL, &cur_val_arr[BEAM_LOW]);       //We are going to the low_beam pwm for a hybrid beam
          *state |= BEAM_LOW_MASK;                               //We'll flip the high_beam flag later, so now we must set the low_beam's one
        }
        else
          beamStop(pin, 0, &cur_val_arr[BEAM_HIGH]);             //We are going to the 0 pwm for a high beam
      else {
        beamStop(pin, BEAM_DAY_VAL, &cur_val_arr[beam]);         //We are going to the day pwm for rear or low beam
        *state |= mask_d;                                        //Setting up the daylights flag
      }
      *state &= ~mask;
    }
    
    //**********
    else if (*state & mask_d) {                                  //if the beam is in a daylight mode
      if (beam == BEAM_HIGH)
        if (BEAM_HYBRID) {
          beamStart(P_BEAM_LOW, BEAM_HIGH_VAL, &cur_val_arr[BEAM_LOW]);       //We are going to the high_beam pwm for a hybrid beam
          *state &= ~BEAM_LOW_MASK;                    //We'll flip the high_beam flag later, so now we must reset the low_beam's one
        }
        else
          beamStart(pin, BEAM_HIGH_VAL, &cur_val_arr[BEAM_HIGH]);              //We are going to the beam_high_val for a high beam
      else {
        beamStart(pin, set_val, &cur_val_arr[beam]); 
      }
      *state &= ~mask_d;
      *state |= mask;
    }

    //**********
    else                                                           //if the beam is turned off
      if ((beam == BEAM_HIGH) && (*state & BEAM_LOW_MASK)) {       //if the beam is high and the low one is turned on
        if (BEAM_HYBRID) {
          beamStart(P_BEAM_LOW, BEAM_HIGH_VAL, &cur_val_arr[BEAM_LOW]);       //We are going to the high_beam pwm for a hybrid beam
          *state &= ~BEAM_LOW_MASK;                    
        }
        else
          beamStart(pin, BEAM_HIGH_VAL, &cur_val_arr[BEAM_HIGH]);              //We are going to the beam_high_val for a high beam
        *state |= mask;
      }
}

/* ToDo:
 *  Add sleep mode sequence when all lights are going down.
 *  Activate interrupt vector for a power button when going to sleep mode.
 *  Disable mentioned above interrupt after wakining up.
 *      Actually, this isn't related to this "test" firmware.
 */
void beamDaySwitch(byte* state, byte* cur_val_arr) {    //Turn on or off daylights. Actually, it is powering on/off
  if (*state & BEAM_HIGH_MASK) {  //turning off the high beam, doesn't matter if we turning everything on or off
    beamStop(mapBeamPin(BEAM_HIGH), 0, &cur_val_arr[BEAM_HIGH]);
    *state &= ~(BEAM_HIGH_MASK | (BEAM_HYBRID?BEAM_LOW:0x00));
  }

  if (*state & (BEAM_LOW_MASK | BEAM_LOW_D_MASK)) {     //Turn off low beam
    beamStop(P_BEAM_LOW, 0, &cur_val_arr[BEAM_LOW]);
    *state &= ~(BEAM_LOW_MASK | BEAM_LOW_D_MASK);
  }
  else {                                                //Turn on low beam (day mode)
    beamStart(P_BEAM_LOW, BEAM_DAY_VAL, &cur_val_arr[BEAM_LOW]);
    *state |= BEAM_LOW_D_MASK;
  }

  if (*state & (BEAM_REAR_MASK | BEAM_REAR_D_MASK)) {   //Turn off rear lights
      beamStop(P_BEAM_REAR, 0, &cur_val_arr[BEAM_REAR]);
      *state &= ~(BEAM_REAR_MASK | BEAM_REAR_D_MASK);
  }
  else {                                                //Turn on rear lights (day mode)
    beamStart(P_BEAM_REAR, BEAM_DAY_VAL, &cur_val_arr[BEAM_REAR]);
    *state |= BEAM_REAR_D_MASK;
  }
}

void beamHighFlash(byte* cur_val_arr) {
  
}
