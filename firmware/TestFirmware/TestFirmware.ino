#include <Arduino_FreeRTOS.h>
#include <queue.h>

#include "pins.h"
#include "buttons.h"
#include "readings.h"
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
    while(analogRead(BUTTON) >= BX)  //do nothing for 60ms, while no button is pressed
        vTaskDelay(2);
    state_prev = analogRead(BUTTON); //measure once
    vTaskDelay(2);                   //debounce delay
    state_cur = analogRead(BUTTON);  //measure twice
    state_diff = (state_cur > state_prev)?(state_cur - state_prev):(state_prev - state_cur);  //calculate difference between measurements
    if (state_diff <= BUT_DEBOUNCE)  //some strange debounce check
    {
      while(analogRead(BUTTON) < BX) { //waiting for a button to be released, count "time"
        vTaskDelay(2);
        if (press_ticks < 255) //we don't want to reset a counter when it is already a 255 in it
          press_ticks += 1;
      }
      press_ticks = (press_ticks>=BUT_LONGPRESS)?1:0; //binarizing the pressing time to say is it longpress or not
      button_state = packButtonState(mapButtonNum(state_cur), press_ticks); //mapping button number and packing it with longetivity info into 1 byte
      xQueueSend(buttonQueue, &button_state, portMAX_DELAY);        //send the packed button info
    }
    vTaskDelay(4);
  }
}

void TaskCheckReadings(void* pvParameters){
  word voltage, lux;
  byte pack;

  for(;;)
  {
    voltage = analogRead(VOLTAGE);
    lux = analogRead(PHOTO);
    pack = packNormLuxVoltage(getNormLux(lux), getNormVoltage(voltage));
    xQueueSend(readingsQueue, &pack, portMAX_DELAY);
    vTaskDelay(1000 / portTICK_PERIOD_MS); // we don't need to read lux and voltage info more than once a second
  }
  
}

//*** Functions section ***

int check_voltage() {
  int vol = analogRead(VOLTAGE);
  return vol;
}

int check_lux() {
  int lux = analogRead(PHOTO);
  return lux;
}

void blink() {
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_Y, LOW);
  delay(150);
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_Y, HIGH);
  delay(150);
}

//*** Main section ***

void setup() {
  buttonQueue = xQueueCreate(1, sizeof(byte));
  readingsQueue = xQueueCreate(1, sizeof(byte));

  xTaskCreate(TaskCheckButton, "CheckButton", 128, NULL, 2, NULL);
  xTaskCreate(TaskCheckReadings, "CheckReadings", 128, NULL, 0, NULL);
  Serial.begin(9600);
}

void loop() {}
