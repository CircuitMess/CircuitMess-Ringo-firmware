#include <SD.h>
#include <SPI.h>


void setup(){
    Serial.begin(115200);
    SD.begin();
}

void loop(){
    Serial.print("DVA");

}