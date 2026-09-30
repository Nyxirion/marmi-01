#include "sht_31.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();

void init_sht31(void){
    
    Serial.println("SHT31 test");
    
    if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
        Serial.println("Couldn't find SHT31");
        digitalWrite(LED_SENSOR, HIGH);
    while (1) delay(1);
    }

    // make sure the heater is off
    digitalWrite(LED_SENSOR, LOW);
    
    Serial.print("Heater Enabled State: ");
    if (sht31.isHeaterEnabled())
        Serial.println("ENABLED");
    else
        Serial.println("DISABLED");

};
