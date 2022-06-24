// Author : Deepak Yadu
// Circuit desginer: Aman Singh

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <LiquidCrystal_I2C.h>
#include <Adafruit_BMP085.h>
#define seaLevelPressure_hPa 1013.25

//BMP sensor
Adafruit_BMP085 bmp;

//Motor connections
#define enA 9
#define in1 6

// Set the LCD address to 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define REPORTING_PERIOD_MS     1000

int BPM, SpO2;
float TMP;

//
bool motorOn;
int timer = 0;

PulseOximeter pox;
uint32_t tsLastReport = 0;

void setup() {

//  LCD setup
  // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("Smart O2 COMC.");
  delay(3000);
//  oximeter steup
  Serial.begin(9600);
  pinMode(16, OUTPUT);
  delay(100);
  
  Serial.print("Initializing pulse oximeter..");
  lcd.print("Initializing pulse oximeter..");

  if (!pox.begin()) {
    Serial.println("FAILED");
    lcd.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
    lcd.println("SUCCESS");
  }

// BMP pinModes
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

//  motor pinModes
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  digitalWrite(in1, LOW);
  pinMode(A0, INPUT);
  Serial.begin(9600);
}
void loop() {

  //  control motor
  int potValue = analogRead(A0); // Read potentiometer value
  int pwmOutput = map(potValue, 0, 1023, 0 , 255); // Map the potentiometer value from 0 to 255
  analogWrite(enA, pwmOutput); // Send PWM signal to L298N Enable pin
//  Serial.print("pwmOutput: ");
//  Serial.print(pwmOutput);
//  Serial.println("potValue: ");
//  Serial.print(potValue);
//  delay(20);

  //  oximeter and BMP sensor with LCD
  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

    BPM = pox.getHeartRate();
    SpO2 = pox.getSpO2();
    TMP = (bmp.readTemperature()*1.8)+32;

    Serial.print("BPM: ");
    Serial.println(BPM);

    Serial.print("SpO2: ");
    Serial.print(SpO2);
    Serial.println("%");
    
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    
      //lcd print
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("BPM: ");
      lcd.print(BPM);

      lcd.setCursor(9,0);
      lcd.print("TEMP:*F");
      lcd.setCursor(9,1);
      lcd.print(TMP);
      
      lcd.setCursor(0,1);
      lcd.print("O2: ");
      lcd.print(SpO2);
      lcd.print("%");

      //
      if(SpO2<=95 && SpO2 >=50){
          timer=0;
          motorOn = true;
          digitalWrite(in1, HIGH);
        }   
      if(motorOn == true && timer <= 10){
          timer++;
        }   
      else{
          timer=0;
          motorOn=false;
          digitalWrite(in1, LOW);
        }

    Serial.println("*********************************");
    Serial.println();
    tsLastReport = millis();
  }


}
