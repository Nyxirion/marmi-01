#include <Arduino.h>
#include <TaskScheduler.h>

#include "menu_config/menu_config.h"
#include "sht_31/sht_31.h"

#include "ds18b20/ds18b20.h"

#define LED_PID 45
#define EXTRACTOR 12
#define HUM_RESISTOR 11
#define WATER_LEVEL 6

// constants

#define maxHumidity 80
#define minHumidity 45

#define maxTemperature 39

Scheduler runner;

LiquidCrystal_I2C lcd2(0x26, 2, 16);

// Intervalos (ms)
const unsigned long ON_TIME = 500;
const unsigned long OFF_TIME = 500;

// Estado humedad
unsigned long lastChange = 0;
bool estado = false; // false = apagado, true = encendido

// Estado DS18B20
static uint32_t lastRequest = 0;
static bool pending = false;
float t2 = 0;

// declarations
void testSystem(void);

// PID THINGS

void pid()
{

    float t = sht31.readTemperature();

    if (pidState)
    {
        digitalWrite(LED_PID, !digitalRead(LED_PID));

        temperature = t;

        if (modo == 0)
        {
            uint32_t time = millis();
            uint32_t intervalo = estado ? ON_TIME : OFF_TIME;

            if (temperature > maxTemperature)
            {
                pid_temp.output = 0.0f;
                digitalWrite(33, HIGH);
                if (time - lastChange >= intervalo)
                {
                    estado = !estado;
                    digitalWrite(LED_BUILTIN, estado ? HIGH : LOW);
                    lastChange = time;
                }
            }
            else
            {
                digitalWrite(LED_BUILTIN, LOW); // Buzzer OFF
                digitalWrite(33, LOW);          // Alarm Led OFF
                pid_temp.output = computePID(&pid_temp, temperature);

                analogWrite(9, (int)pid_temp.output);
            }

            // Serial print for debug
            Serial.print("output is:");
            Serial.println(pid_temp.output);
            Serial.println(temperature);
        }

        if(modo){

            uint32_t time2 = millis();
            uint32_t intervalo = estado ? ON_TIME : OFF_TIME;

            if (t2 > maxTemperature)
            {
                pid_temp.output = 0.0f;
                digitalWrite(33, HIGH);
                if (time2 - lastChange >= intervalo)
                {
                    estado = !estado;
                    digitalWrite(LED_BUILTIN, estado ? HIGH : LOW);
                    lastChange = time2;
                }
            }
            else
            {
                digitalWrite(LED_BUILTIN, LOW); // Buzzer OFF
                digitalWrite(33, LOW);          // Alarm Led OFF
                pid_temp.output = computePID(&pid_temp, t2);

                analogWrite(9, (int)pid_temp.output);
            }

            // Serial print for debug
            Serial.print("output is:");
            Serial.println(pid_temp.output);
            Serial.println(t2);
            
        }
        updateDisplay();
    }

    else
    {
        analogWrite(9, 0);
        digitalWrite(LED_PID, LOW);
    }
}

Task taskSystemTest(200, TASK_FOREVER, &testSystem);

void humidity()
{
    if (systemTest)
        taskSystemTest.enable();

    hum = (int)sht31.readHumidity();

    if (hum < set_hum - 5)
    {
        digitalWrite(HUM_RESISTOR, HIGH);
    }
    else
    {
        digitalWrite(HUM_RESISTOR, LOW);
    }
    if (hum >= maxHumidity)
    {
        digitalWrite(HUM_RESISTOR, LOW);
    }

    if (digitalRead(WATER_LEVEL))
    {
        digitalWrite(30, HIGH);
    }
    else
    {
        digitalWrite(30, LOW);
    }
}

void ds18b20()
{

    uint32_t now = millis();

    if (!pending)
    {
        ds18b20_readTemperature();
        lastRequest = now;
        pending = true;
    }

    else if (now - lastRequest >= 750)
    {
        t2 = ds18b20_getTemperature();

        lcd2.clear();
        lcd2.print(t2);
        pending = false;
    }
}
Task taskButtons(25, TASK_FOREVER, &buttonObserver);
Task taskPID(2000, TASK_FOREVER, &pid);
Task taskHumidity(1000, TASK_FOREVER, &humidity);
Task taskDs18b20(800, TASK_FOREVER, &ds18b20);

void setup()
{
    Serial.begin(9600);
    pinMode(LED_PID, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(HUM_RESISTOR, OUTPUT);
    pinMode(EXTRACTOR, OUTPUT);
    pinMode(LED_SENSOR, OUTPUT);
    // Eliminar adelante
    pinMode(11, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(33, OUTPUT);

    // Debug de test
    pinMode(28, OUTPUT); // LED FALLA SENSOR
    // digitalWrite(33, HIGH); Esto es un botón
    pinMode(35, OUTPUT); // LED TEMP PIEL
    pinMode(32, OUTPUT); // LED SIN NOMBRE
    pinMode(31, OUTPUT); // FALLA DE RESISTENCIA LED
    pinMode(30, OUTPUT); // WATER LEVEL
    pinMode(45, OUTPUT); // MODO PIEL // Actualmente es el heartbeat del PID
    pinMode(44, OUTPUT); // LED MODE AIRE
    pinMode(42, OUTPUT);
    pinMode(WATER_LEVEL, INPUT);
    // PID INITIAL CONFIG
    pid_temp.kp = 0.8f;
    pid_temp.ki = 0.05f;
    pid_temp.kd = 40.0f;

    pid_temp.setpoint = 35.0f;
    pid_temp.filterTau = 10.0f;
    pid_temp.sampleTime = 2.0f;

    pid_temp.minOutputLim = 0.0f;
    pid_temp.maxOutputLim = 255.0f;

    // Hum
    set_hum = minHumidity;

    pid_config_init(&pid_temp);

    setMenu();
    initButtons();

    init_sht31();
    ds18b20_init();

    lcd2.init();
    lcd2.backlight();

    runner.addTask(taskButtons);
    runner.addTask(taskPID);
    runner.addTask(taskHumidity);
    runner.addTask(taskSystemTest);
    runner.addTask(taskDs18b20);

    taskButtons.enable();
    taskPID.enable();
    taskHumidity.enable();
    taskSystemTest.disable();
    taskDs18b20.enable();
}

void loop()
{
    runner.execute();
}

void testSystem()
{
    if (systemTest)
    {
        taskHumidity.disable();
        taskPID.disable();

        digitalWrite(9, HIGH);           // Enable Resistor PID
        digitalWrite(10, HIGH);          // Relé 1
        digitalWrite(11, HIGH);          // Relé 2
        digitalWrite(12, HIGH);          // Relé 3
        digitalWrite(LED_BUILTIN, HIGH); // Buzzer
        digitalWrite(28, HIGH);          // LED FALLA SENSOR
        // digitalWrite(33, HIGH); Esto es un botón
        digitalWrite(35, HIGH); // LED TEMP PIEL
        digitalWrite(32, HIGH); // LED SIN NOMBRE
        digitalWrite(31, HIGH); // FALLA DE RESISTENCIA LED
        digitalWrite(30, HIGH); // WATER LEVEL
        digitalWrite(45, HIGH); // MODO PIEL // Actualmente es el heartbeat del PID
        digitalWrite(44, HIGH); // LED MODE AIRE
        digitalWrite(42, HIGH); //
    }
    else
    {
        digitalWrite(9, LOW);           // Enable Resistor PID
        digitalWrite(10, LOW);          // Relé 1
        digitalWrite(11, LOW);          // Relé 2
        digitalWrite(12, LOW);          // Relé 3
        digitalWrite(LED_BUILTIN, LOW); // Buzzer
        digitalWrite(28, LOW);          // LED FALLA SENSOR
        // digitalWrite(33, HIGH); Esto es un botón
        digitalWrite(35, LOW); // LED TEMP PIEL
        digitalWrite(32, LOW); // LED SIN NOMBRE
        digitalWrite(31, LOW); // FALLA DE RESISTENCIA LED
        digitalWrite(30, LOW); // WATER LEVEL
        digitalWrite(45, LOW); // MODO PIEL // Actualmente es el heartbeat del PID
        digitalWrite(44, LOW); // LED MODE AIRE
        digitalWrite(42, LOW); //
        taskHumidity.enable();
        taskPID.enable();
        taskSystemTest.disable();
    }
}