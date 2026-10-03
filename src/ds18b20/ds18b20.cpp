#include "ds18b20.h"


static OneWire oneWire(ONE_WIRE_BUS);
static DallasTemperature sensors(&oneWire);

void ds18b20_init() {
    sensors.begin();
}

void ds18b20_readTemperature() {
    sensors.requestTemperatures();
}

float ds18b20_getTemperature(){
    return sensors.getTempCByIndex(0);
}