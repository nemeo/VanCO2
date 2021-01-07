// Preheat the sensor for 24h and run this code to get the RZero value
// Change this value in MQ135.h:
//    ///Calibration resistence at atmospheric CO2 level
//    #define RZERO 5804.99
//
// You also have to set the RLOAD value to the one corresponding on your device:
//    ///The load resistance on the board
//    #define RLOAD 22.0
//
// Finaly set the atmospheric CO2 level (Geneva septembre 2020: 414.0)
//    ///Atmospheric CO2 level for calibration purposes
//    #define ATMOCO2 414.00

#include "MQ135.h"
void setup (){
Serial.begin (9600);
}
void loop() {
MQ135 gasSensor = MQ135(A0); // Attach sensor to pin A0
float rzero = gasSensor.getRZero();
Serial.println (rzero);
delay(1000);
}
