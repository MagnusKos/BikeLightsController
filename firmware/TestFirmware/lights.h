#pragma once

#include "pins.h"

// *** PWM values section ***

#define BEAM_LOW_VAL     72
#define BEAM_HIGH_VAL   255
#define BEAM_REAR_VAL   100

#define BEAM_LOW_D_VAL   20    //Low beam daymode power
#define BEAM_REAR_D_VAL  32    //Rear lights daymode power
#define BEAM_HIGH_D_VAL   0    //High lights daymode power, obviously it is a zero, but if you wish...

#define BEAM_HIGH_THRES  64    //Minimum value for reversing flash action from "old_val-255-old_val" to "old_val-0-old_val"

// *** Special features section

#define BEAM_HYBRID     true   //If high and low beams are same


// *** Helper definitions ***

#define BEAM_LOW          0
#define BEAM_HIGH         1
#define BEAM_REAR         2
  
#define BEAM_SOFT_START true   //Slowly rise PWM value

#ifdef BEAM_SOFT_START
  #define BEAM_SS_DELAY   30 / portTICK_PERIOD_MS  //Rising PWM delay, ms
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
 *    a h l r | - H L R
 *  
 *  H - high beam, if set - turned on according to the table of values
 *  L - low beam, if set - turned on according to the table of values
 *  R - rear lights, if set - turned on according to the table of values
 *  a - set if adaptive mode is on
 *  h - set if high beam is turned off and able to be turned on; not an actual daymode flag
 *  l - set if low beam in a day mode
 *  r - set if rear beam in a day mode
 */

//Power state-half flags
#define BEAM_HIGH_MASK      0b00000100
#define BEAM_LOW_MASK       0b00000010
#define BEAM_REAR_MASK      0b00000001

//Mode-half flags, not in use yet
#define BEAM_ADAPT_MASK     0b10000000
#define BEAM_HIGH_D_MASK    0b01000000
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
      return BEAM_HIGH_MASK;
        
    case BEAM_REAR:
      return BEAM_REAR_MASK;
      
    default:
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
      return BEAM_HIGH_D_MASK;

    default:
      return 0b00000000;        //What is this? I don't know! [1]
  }
}

byte mapBeamVal(byte beam){  //function returns const PWM value by entering the beam num
  switch(beam){
    case BEAM_LOW:
      return BEAM_LOW_VAL;

    case BEAM_HIGH:
      return BEAM_HIGH_VAL;
    
    case BEAM_REAR:
      return BEAM_REAR_VAL;
      
    default:
      return 0b00000000;        //What is this? I don't know! [2]
  }
}

byte mapBeamDayVal(byte beam){ //function returns const PWM daytime value by entering the beam num
    switch(beam){
    case BEAM_LOW:
      return BEAM_LOW_D_VAL;

    case BEAM_HIGH:
      return BEAM_HIGH_D_VAL;
    
    case BEAM_REAR:
      return BEAM_REAR_D_VAL;
      
    default:
      return 0b00000000;        //What is this? I don't know! [3]
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
  *cur_val -= *cur_val - fin_val;    //Subtracting the residue
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
  byte day_val = mapBeamDayVal(beam);
  byte pin = mapBeamPin(beam);

    if (*state & mask) {     //If in a normal mode
      if ((beam == BEAM_HIGH)&&(BEAM_HYBRID)) {                             //Hybrid high beam case, turning it off
        beamStop(P_BEAM_LOW, BEAM_LOW_VAL, &cur_val_arr[BEAM_LOW]);         //We are going to the low_beam pwm for a hybrid beam
        *state |= BEAM_LOW_MASK;                                            //Low beam in a normal mode
      }
      else {                                                                //Any other beam case, switch it to a daymode
        beamStop(pin, day_val, &cur_val_arr[beam]);                         //We are going to the day pwm for rear or low beam
        *state &= ~BEAM_HIGH_D_MASK;                                        //This flag prevents the high beam to be turned on
      }
      *state &= ~mask;                                                      //Reset normal mode flag for the current beam
      *state |= mask_d;                                                     //Set daymode flag for the current beam
      return;
    }

    if (*state & mask_d) {   //If in a daymode
      if ((beam == BEAM_HIGH)&&(BEAM_HYBRID)) {                             //Hybrid high beam case, turning it on
        beamStart(P_BEAM_LOW, BEAM_HIGH_VAL, &cur_val_arr[BEAM_LOW]);       //We are going to the high_beam pwm for a hybrid beam
        *state &= ~(BEAM_LOW_MASK | BEAM_LOW_D_MASK);                       //Low beam neither in a daymode nor in a normal mode
      }
      else {                                                                //Any other beam case, switch it to a normal mode
        beamStart(pin, set_val, &cur_val_arr[beam]);                        //We are going to the normal pwm for rear or low beam
        *state |= BEAM_HIGH_D_MASK;                                         //This flag allows the high beam to be turned on
      }
      *state &= ~mask_d;                                                    //Reset daymode flag for the current beam
      *state |= mask;                                                       //Set normal mode flag for the current beam
      return;
    }
}

/* ToDo:
 *  Add sleep mode sequence when all lights are going down.
 *  Activate interrupt vector for a power button when going to sleep mode.
 *  Disable mentioned above interrupt after wakining up.
 *      Actually, this isn't related to this "test" firmware.
 */
void beamDaySwitch(byte* state, byte* cur_val_arr) {    //Turn on or off daylights. Actually, it is powering on/off
  if (*state & (BEAM_HIGH_MASK | BEAM_HIGH_D_MASK)) {   //Turn everything off if it is a high beam state
    beamStop(mapBeamPin(BEAM_HIGH), 0, &cur_val_arr[BEAM_HYBRID?BEAM_LOW:BEAM_HIGH]);
    beamStop(P_BEAM_LOW, 0, &cur_val_arr[BEAM_LOW]);
    beamStop(P_BEAM_REAR, 0, &cur_val_arr[BEAM_REAR]);
    *state &= ~(BEAM_HIGH_MASK | BEAM_HIGH_D_MASK | BEAM_LOW_MASK | BEAM_LOW_D_MASK | BEAM_REAR_MASK | BEAM_REAR_D_MASK);
    return;
  }

  if (*state & (BEAM_LOW_MASK | BEAM_LOW_D_MASK)) {     //Turn off low beam
    beamStop(P_BEAM_LOW, 0, &cur_val_arr[BEAM_LOW]);
    *state &= ~(BEAM_LOW_MASK | BEAM_LOW_D_MASK);
  }
  else {                                                //Turn on low beam (day mode)
    beamStart(P_BEAM_LOW, BEAM_LOW_D_VAL, &cur_val_arr[BEAM_LOW]);
    *state |= BEAM_LOW_D_MASK;
  }

  if (*state & (BEAM_REAR_MASK | BEAM_REAR_D_MASK)) {   //Turn off rear lights
      beamStop(P_BEAM_REAR, 0, &cur_val_arr[BEAM_REAR]);
      *state &= ~(BEAM_REAR_MASK | BEAM_REAR_D_MASK);
  }
  else {                                                //Turn on rear lights (day mode)
    beamStart(P_BEAM_REAR, BEAM_REAR_D_VAL, &cur_val_arr[BEAM_REAR]);
    *state |= BEAM_REAR_D_MASK;
  }
}

void beamHighFlash(byte* cur_val_arr) {
  
}
