#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#include "menu_config/menu_config.h"

void setup(){
    Serial.begin(9600);
    setMenu();
    initButtons();

}

void loop(){
    buttonObserver();
}