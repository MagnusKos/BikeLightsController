#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#include "pins.h"
#include "buttons.h"
#include "readings.h"
#include "lights.h"
#include "communication.h"

#include "functions.h"


// *** Variables section ***

QueueHandle_t buttonQueue;
QueueHandle_t readingsQueue;
QueueHandle_t debugQueue;

byte beam_pwm[3];            //current pwm values, use with beamMutex
byte beam_state;             //bitmap with state flags, see "lights.h"
SemaphoreHandle_t beamMutex;

byte com_tx_buff[COM_BUFF_SIZE];

// *** Tasks section ***

void TaskCheckButton(void *pvParameters) {
  (void) pvParameters;
  
  word state_cur, state_prev, state_diff, press_ticks=0;
  byte button_state;
  for (;;)
  {
    while(analogRead(P_BUTTON_MUL) >= BX)  //do nothing for 60ms, while no button is pressed
        vTaskDelay(2);
    state_prev = analogRead(P_BUTTON_MUL); //measure once
    vTaskDelay(2);                   //debounce delay
    state_cur = analogRead(P_BUTTON_MUL);  //measure twice
    state_diff = (state_cur > state_prev)?(state_cur - state_prev):(state_prev - state_cur);  //calculate difference between measurements
    if (state_diff <= BUT_DEBOUNCE)  //some strange debounce check
    {
      while(analogRead(P_BUTTON_MUL) < BX) { //waiting for a button to be released, count "time"
        vTaskDelay(2);
        if (press_ticks < 255) //we don't want to reset a counter when it is already a 255 in it
          press_ticks += 1;
      }
      press_ticks = (press_ticks >= BUT_LONGPRESS)?1:0; //binarizing the pressing time to say is it longpress or not
      button_state = packButtonState(mapButtonNum(state_cur), press_ticks); //mapping button number and packing it with longetivity info into 1 byte
      xQueueSend(buttonQueue, &button_state, 2); //send the packed button info, we don't need to wait too long to send the command
    }
    vTaskDelay(4);
  }
}

void TaskCheckReadings(void* pvParameters){
  word voltage, lux;
  byte pack;

  for(;;)
  {
    voltage = analogRead(P_VOLTAGE);
    lux = analogRead(P_LUX);
    pack = packNormLuxVoltage(getNormLux(lux), getNormVoltage(voltage));
    xQueueSend(readingsQueue, &pack, portMAX_DELAY);
    vTaskDelay(1000 / portTICK_PERIOD_MS); // we don't need to read lux and voltage info more than once a second
  }
  
}

/* ToDo
 *  This task is definitely part of the button task, because it should "decode"
 *  the pressed button code and generate some command to execute. It waits for a data from button queue indefinitely,
 *  so it can't run adaptive lights.
 */
void TaskCommandsWorker(void* pvParameters){
  UBaseType_t uxHighWaterMark;
  byte beam_temp_pwm; // temporary pwm value
  byte button;
  uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
  xQueueSend(debugQueue, &uxHighWaterMark, 2);
  for(;;)
  {
    if (xQueueReceive(buttonQueue, &button, portMAX_DELAY) == pdPASS) {
      if (xSemaphoreTake(beamMutex, portMAX_DELAY) == pdTRUE) {                   //protect PWM vars from overwrite from another task
        switch(button) {
          
          case ACT_FLASH:
            if (beam_state) {
              beam_temp_pwm = (beam_pwm[BEAM_HYBRID?BEAM_LOW:BEAM_HIGH] > BEAM_HIGH_THRES)?0:255;
              analogWrite(mapBeamPin(BEAM_HIGH), beam_temp_pwm);                              //turn fully on or off
              beam_state ^= BEAM_HIGH_MASK;
              vTaskDelay(100 / portTICK_PERIOD_MS);                                           //wait for ~100ms
              analogWrite(mapBeamPin(BEAM_HIGH), beam_pwm[BEAM_HYBRID?BEAM_LOW:BEAM_HIGH]);   //restore previous power
              beam_state ^= BEAM_HIGH_MASK;
            }
            break;
            
          case ACT_HIGH_BEAM:
            beamSwitch(BEAM_HIGH, &beam_state, beam_pwm);
            break;

          case ACT_LOW_BEAM:
            if (beam_state & BEAM_HIGH_MASK) {
              beamSwitch(BEAM_HIGH, &beam_state, beam_pwm);
            }
            else {
              beamSwitch(BEAM_LOW, &beam_state, beam_pwm);
              beamSwitch(BEAM_REAR, &beam_state, beam_pwm);
            }
            break;

          case ACT_POWER:
            beamDaySwitch(&beam_state, beam_pwm); //ToDo: test this thing
            break;

          case ACT_ADAPTIVE: //ToDo: create this thing

            break;
        
        }
        xSemaphoreGive(beamMutex); //we don't need to modify PWM vars anymore
      }
    }
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    xQueueSend(debugQueue, &uxHighWaterMark, 2);
    vTaskDelay(2);
  }
}

void TaskAdaptiveWorker(void* pvParameters){
  for (;;) {
    
  }
  
}

void TaskIndication(void* pvParameters){
  for (;;) {
    
  }
}

void TaskPrintToSerial(void* pvParameters){
  Serial.begin(9600);
  while (!Serial) {
    vTaskDelay(2);
  }

  byte button, readings;
  UBaseType_t mark;
  com_tx_buff[COM_TX_SIG_MSB] = COM_SIG_MSB;
  com_tx_buff[COM_TX_SIG_LSB] = COM_SIG_LSB;

  for (;;) {
    if (xQueueReceive(readingsQueue, &readings, 2) == pdPASS) {
      com_tx_buff[COM_TX_READINGS] = readings;
    }
    if (xQueueReceive(debugQueue, &mark, 2) == pdPASS) {
      com_tx_buff[COM_TX_DBG] = mark;
    }
    
    if (xSemaphoreTake(beamMutex, portMAX_DELAY) == pdTRUE) {
      com_tx_buff[COM_TX_HIGH] = beam_pwm[BEAM_HIGH];
      com_tx_buff[COM_TX_LOW] = beam_pwm[BEAM_LOW];
      com_tx_buff[COM_TX_REAR] = beam_pwm[BEAM_REAR];
      com_tx_buff[COM_TX_STATE] = beam_state;
      
      xSemaphoreGive(beamMutex);
    }
    Serial.write(com_tx_buff, COM_BUFF_SIZE);
    vTaskDelay(33);
  }
}

// *** Functions section ***


// *** Main section ***

void setup() {
  buttonQueue = xQueueCreate(1, sizeof(byte));
  readingsQueue = xQueueCreate(1, sizeof(byte));

  debugQueue = xQueueCreate(1, sizeof(UBaseType_t));

  beamMutex = xSemaphoreCreateMutex();
  beam_state = 0b00000000;
  zeroArray(beam_pwm, 3);

  xTaskCreate(TaskCheckButton, "CheckButton", 50, NULL, 2, NULL);        //50 is enougth, 7 words are "in a pocket"
  xTaskCreate(TaskCheckReadings, "CheckReadings", 64, NULL, 0, NULL);    //64, 7 are free
  xTaskCreate(TaskCommandsWorker, "CommandsWorker", 64, NULL, 2, NULL);  //64, 4 are free
  xTaskCreate(TaskPrintToSerial, "PrintSerial", 128, NULL, 2, NULL);
}

void loop() {}
