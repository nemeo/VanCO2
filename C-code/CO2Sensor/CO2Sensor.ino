/*
 * MQ135 Gas Senor and DHT11 with Arduino NaNo
 * Author: Nemeo
 * Website: www.lagrossemiche.fr
 * Date: 20-01-2021
 * 
 * The goal of the setup is to set an alarm if the CO2 level exceed 1% in our van.
 * It will start a fan at 0.5% and drive an allarm at 1%.
 * The atmospheric levet is normally 0.041% (414ppm).
 * The nocivity effect of the CO2 start at 2% and it must not exceed 3% for 15 minutes.
 * At 15% it is sudden loss of consciousness and at 25%, respiratory arrest results in death.
 */
#include <TimeInterrupt.h>

/**********************************************************/
/*                the DHT Sensor                          */
/**********************************************************/

#include <DHT.h>
#define DHTPIN 4        //Pin of DHT 11
#define DHTTYPE DHT11   //Type of DHT
DHT sensor_dht(DHTPIN, DHTTYPE);
float temperature, humidity;
bool readDHT = true;

/**********************************************************/
/*                the MQ135 Sensor                        */
/**********************************************************/

// The load resistance on the board
#define RLOAD 22.0
#include "MQ135.h"
MQ135 gasSensor = MQ135(A0);
int val;
int sensorValue = 0;
float ppm = 0.0;

/**********************************************************/
/*                the OLED Display                        */
/**********************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
// Declaration for SSD1306 display connected using software i2c:
#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
bool screenLatch = true;

const int buzzer = 8;


/**********************************************************/
/*                the Roary Encoder                       */
/**********************************************************/

#include <Encoder.h>

#define encoderCLK 2  // Rotary clock
#define encoderDT 6   // Rotary data   
#define encoderSW 5   //Rotary switch

Encoder myEnc(encoderCLK, encoderDT);
long oldPosition  = -999;
bool pressSW = false;
String printDT = "", printDir = "", printSW = "";

void bip(){
  tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(200);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
}
void updateDHT (){
  // Read temperature as Celsius
  temperature = sensor_dht.readTemperature();
  // Read humidity
  humidity = sensor_dht.readHumidity();
  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    temperature = 0.0;
    humidity = 0.0;
    return;
  }
  else{
    Serial.println ("Temp: " + ((String)temperature) + "C");
    Serial.println ("Humi: " + ((String)humidity) + "%");
  }
}
void doEnter(){
  bip();bip();bip();
}
void readSensors(){
  readDHT = true;
  ppm = gasSensor.getPPM();
  val = analogRead(A0); 
  Serial.print ("raw = ");
  Serial.println (val);
 // float zero = gasSensor.getRZero();
 // Serial.print ("rzero: ");
  //Serial.println (zero);
  Serial.print ("ppm: ");
  Serial.println (ppm);
}
void setup() {
  Serial.begin(9600);
  //init DHT11
  sensor_dht.begin();
  //init MQ135
  pinMode(A0, INPUT);
  //init display
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.display();
  //init buzzer
  pinMode(buzzer, OUTPUT);
  bip();bip();
  pinMode(encoderSW, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(encoderSW), doEnter, CHANGE);

  
  TimeInterrupt.begin(PRECISION); //Initialize the interrupt with high precision timing
  TimeInterrupt.addInterrupt(readSensors, 2000);
  readSensors();
}
void loop() {
  if (readDHT){
    updateDHT ();
    readDHT = false;
  }
  long newPosition = myEnc.read();
  uint8_t line = 0, col = 0;
  if (newPosition %4 == 0){
    if (newPosition != oldPosition) {
      if(newPosition < oldPosition)
      {
        printDir = " CW";
      }
      
      if(newPosition > oldPosition)
      {
        printDir = " CCW";
      }
      oldPosition = newPosition;
      printDT = "SW= " + String(newPosition/4);
    }
  }
  if (digitalRead(encoderSW)==1){
    if (pressSW){
       screenLatch = !screenLatch;
       pressSW = false;
    }
    printSW = " Off";
  }
  else{
    pressSW = true;
    printSW = " On";
  }
  
  if (screenLatch){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(col,line);line += 8;
    display.println("CO2:  " + ((String)ppm) + "ppm");
    display.setCursor(col,line);line += 8;
    display.println("Temp: " + ((String)temperature) + "C");
    display.setCursor(col,line);line += 8;
    display.println("Humi: " + ((String)humidity) + "%");
    display.setCursor(col,line);
    display.println(printDT + printDir + printSW);  
    display.display();
  }
  else{
    display.clearDisplay();
    display.display();
  }
  delay(250);
}
