#include <Arduino_FreeRTOS.h>
#include <queue.h>

#include "pins.h"
#include "buttons.h"
#include "readings.h"
#include "lights.h"
//#include "functions.h"

QueueHandle_t buttonQueue;
QueueHandle_t readingsQueue;

//*** Tasks section ***

void TaskCheckButton(void *pvParameters) {
  (void) pvParameters;
  
  word state_cur, state_prev, state_diff, press_ticks=0;
  byte button_state;
  for (;;)
  {
    while(analogRead(P_BUTTON) >= BX)  //do nothing for 60ms, while no button is pressed
        vTaskDelay(2);
    state_prev = analogRead(P_BUTTON); //measure once
    vTaskDelay(2);                   //debounce delay
    state_cur = analogRead(P_BUTTON);  //measure twice
    state_diff = (state_cur > state_prev)?(state_cur - state_prev):(state_prev - state_cur);  //calculate difference between measurements
    if (state_diff <= BUT_DEBOUNCE)  //some strange debounce check
    {
      while(analogRead(P_BUTTON) < BX) { //waiting for a button to be released, count "time"
        vTaskDelay(2);
        if (press_ticks < 255) //we don't want to reset a counter when it is already a 255 in it
          press_ticks += 1;
      }
      press_ticks = (press_ticks>=BUT_LONGPRESS)?1:0; //binarizing the pressing time to say is it longpress or not
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
void TaskLightsWorker(void* pvParameters){
  byte beam_h, beam_l, beam_r; //current pwm values
  byte button;
  byte state; //bitmap with state flags, see "lights.h"
  for(;;)
  {
    if (xQueueReceive(buttonQueue, &button, portMAX_DELAY) == pdPASS) {
      switch(button) {
        case ACT_FLASH:
          analogWrite(BEAM_HIGH, 255);
          vTaskDelay(100 / portTICK_PERIOD_MS);
          analogWrite(BEAM_HIGH, 0);
          break;
        case ACT_HIGH_BEAM:
          break;
      }
    }
  }
}

void TaskPrintToSerial(void* pvParameters){
  Serial.begin(9600);
  while (!Serial) {
    vTaskDelay(1);
  }

  byte button, readings;

  for (;;) {
    if (xQueueReceive(buttonQueue, &button, 2) == pdPASS) {
      Serial.print("Button: ");
      Serial.println(button);
    }
    if (xQueueReceive(readingsQueue, &readings, 2) == pdPASS) {
      Serial.print("Readings: ");
      Serial.println(readings, BIN);
    }
  }
}

//*** Functions section ***


//*** Main section ***

void setup() {
  buttonQueue = xQueueCreate(1, sizeof(byte));
  readingsQueue = xQueueCreate(1, sizeof(byte));

  xTaskCreate(TaskCheckButton, "CheckButton", 50, NULL, 2, NULL);     //50 is enougth, 7 words are "in a pocket"
  xTaskCreate(TaskCheckReadings, "CheckReadings", 64, NULL, 0, NULL); //64, 7 are free
  xTaskCreate(TaskPrintToSerial, "PrintSerial", 128, NULL, 2, NULL);
}

void loop() {}
