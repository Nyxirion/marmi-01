#include <Arduino.h>

#include "menu_config/menu_config.h"

void setup(){
    Serial.begin(9600);
    setMenu();
    initButtons();

}

void loop(){
    buttonObserver();
}