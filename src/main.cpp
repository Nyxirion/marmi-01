#include <Arduino.h>
#include <TaskScheduler.h>

#include "menu_config/menu_config.h"
#include "sht_31/sht_31.h"

#define LED_PID 45
#define EXTRACTOR 12
#define HUM_RESISTOR 10

// constants

#define maxHumidity 80
#define minHumidity 40

#define maxTemperature 39

Scheduler runner;

//PID THINGS

void pid(){

    float temperature = sht31.readTemperature();

    if(pidState){
        digitalWrite(LED_PID, !digitalRead(LED_PID));

        if(temperature > maxTemperature){
            pid_temp.output = 0.0f;
        }
        else{
            pid_temp.output = computePID(&pid_temp, temperature);

            analogWrite(9, (int)pid_temp.output);
        }
        
        // Serial print for debug
        Serial.print("output is:");
        Serial.println(pid_temp.output);
        Serial.println(temperature);
        
        updateDisplay();
    }
}

void humidity(){
    float hum = sht31.readHumidity();

    if(hum >= maxHumidity){
        digitalWrite(EXTRACTOR, HIGH);
        digitalWrite(HUM_RESISTOR, LOW);
    }
    
    if(hum <= minHumidity){
        digitalWrite(EXTRACTOR, LOW);
        digitalWrite(HUM_RESISTOR, HIGH);
    }
    
    
}
Task taskButtons(25, TASK_FOREVER, &buttonObserver);
Task taskPID(2000, TASK_FOREVER, &pid);
Task taskHumidity(1000, TASK_FOREVER, &humidity);

void setup(){
    Serial.begin(9600);
    pinMode(LED_PID, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(HUM_RESISTOR, OUTPUT);
    pinMode(EXTRACTOR, OUTPUT);

    //PID INITIAL CONFIG
    pid_temp.kp = 0.8f;
    pid_temp.ki = 0.05f;
    pid_temp.kd = 40.0f;

    pid_temp.setpoint = 35.0f;
    pid_temp.filterTau = 10.0f;
    pid_temp.sampleTime = 0.5f;

    pid_temp.minOutputLim = 0.0f;
    pid_temp.maxOutputLim = 255.0f;

    
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

