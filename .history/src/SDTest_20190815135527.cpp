#include <SD.h>
#include <SPI.h>

SDFS sd;

void setup(){
    Serial.begin(115200);
}

void loop(){
    Serial.print(sd.cardSize);
    
}