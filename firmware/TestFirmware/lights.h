// *** PWM values section ***

#define BEAM_LOW_VAL    128
#define BEAM_HIGH_VAL   255
#define BEAM_REAR_VAL   128
#define BEAM_DAY_VAL     64

// *** Special features section

#define BEAM_HYBRID     true   //If high and low beam is the same

#ifdef BEAM_HYBRID
  #define BEAM_HIGH P_BEAM_LOW
#elif
  #define BEAM_HIGH P_BEAM_HIGH
#endif
  
#define BEAM_SOFT_START true   //Slowly rise PWM value

#ifdef BEAM_SOFT_START
  #define BEAM_SS_DELAY   100 / portTICK_PERIOD_MS  //Rising PWM delay, ms
#endif

// *** Adaptive mode section ***

/* The value from a photoresistor is normalized and packed into 4 bits, 
 *  so there are only 16 (0..15) levels of luminosity.
 *  Insert values that define, which beam (or power applied to one)
 *  should be selected automatically.
 */
#define BEAM_AD_DAY_THRES    0
#define BEAM_AD_LOW_THRES    0
#define BEAM_AD_HIGH_THRES   0

#define BEAM_AD_HYST         2000 / portTICK_PERIOD_MS //Hysteresis time, prevents strobing, while riding
                                                       // from shadows to lights multiple times

// *** States section ***

/* Here we have one byte bitmap with flags, representing binary states.
 *   FEATURES |  PWMS
 *  [ 0 0 0 0 | 0 0 0 0 ]
 *              d H L R
 *  d - direction, rise (1) or lower (0)
 *  H - high beam, if set - manipulate its PWM according to the 'd' flag
 *  L - low beam, if set - manipulate its PWM according to the 'd' flag
 *  R - rear lights, if set - manipulate its PWM according to the 'd' flag
 */

// 
