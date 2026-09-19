#include <Arduino.h>
#include <TaskScheduler.h>

#include "menu_config/menu_config.h"
#include "pid.h"
#include "sht_31/sht_31.h"

Scheduler runner;


void pid(){

    float t = sht31.readTemperature();

    if(pidState){
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        temperature = t;
        Serial.println(temperature);
        updateDisplay();
    }
}
Task taskButtons(25, TASK_FOREVER, &buttonObserver);
Task taskPID(500, TASK_FOREVER, &pid);

void setup(){
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    setMenu();
    initButtons();

    init_sht31();

    runner.addTask(taskButtons);
    runner.addTask(taskPID);

    taskButtons.enable();
    taskPID.enable();
}

void loop(){
    runner.execute();
}

