#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 8


void ds18b20_init();

void ds18b20_readTemperature();

float ds18b20_getTemperature();