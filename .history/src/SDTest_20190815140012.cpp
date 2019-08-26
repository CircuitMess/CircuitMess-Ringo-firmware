#include <SD.h>
#include <SPI.h>

extern SDFS SD;

void setup(){
    Serial.begin(115200);
    SD.begin();
}

void loop(){
    

}