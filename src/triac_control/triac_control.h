#pragma once

#include <Arduino.h>

// ===== Pines =====
#define ZERO_CROSS_PIN 2
#define TRIAC_PIN 9

extern volatile unsigned int intensidad;
extern volatile unsigned int intensidad_vista_por_isr;

void triacConfigSetup();