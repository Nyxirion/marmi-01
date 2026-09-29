#include <Arduino.h>
#include <TaskScheduler.h>

#include "menu_config/menu_config.h"
#include "sht_31/sht_31.h"

Scheduler runner;

//PID THINGS

void pid(){

    float t = sht31.readTemperature();

    if(pidState){
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        temperature = t;


        pid_temp.output = computePID(&pid_temp, temperature);

        analogWrite(9, (int)pid_temp.output);

        
        // Serial print for debug
        Serial.print("output is:");
        Serial.println(pid_temp.output);
        Serial.println(temperature);
        updateDisplay();
    }
}
Task taskButtons(25, TASK_FOREVER, &buttonObserver);
Task taskPID(500, TASK_FOREVER, &pid);

void setup(){
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    //PID INITIAL CONFIG
    pid_temp.kp = 0.8f;
    pid_temp.ki = 0.05f;
    pid_temp.kd = 0;
    pid_temp.setpoint = 35.0f;

    pid_temp.minOutputLim = 0.0f;
    pid_temp.maxOutputLim = 255.0f;

    pid_temp.sampleTime = 0.5f;
    pid_config_init(&pid_temp);


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

