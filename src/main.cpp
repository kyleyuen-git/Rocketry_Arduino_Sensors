#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPL3115A2.h>

Adafruit_MPL3115A2 mpl;
void dataCollection();
void WireSetup();



void setup() {
  WireSetup();
  
}

void loop() {
  dataCollection();

}

void WireSetup(){
  Serial.begin(115200);
  delay(2000);

  Serial.println("Starting MPL3115A2...");

  // I2C explicit
  Wire.begin();
  delay(10);

  // while(!mpl.begin(&Wire)){
  //   Serial.println("Sensor not found (begin failed)");
  //   delay(100);
  //   Wire.begin();
  // }

  if (!mpl.begin(&Wire)) {
    Serial.println("Sensor not found (begin failed)");
    while (1) delay(10);
  }

  Serial.println("Sensor found!");

  // optional: setting sea level pressure for better altitude
  mpl.setSeaPressure(1013.25);
}

void dataCollection(){
  float alt = mpl.getAltitude();
  float pres = mpl.getPressure();
  float temp = mpl.getTemperature();

  Serial.print("Alt(m): "); Serial.print(alt, 2);
  Serial.print("  P(hPa): "); Serial.print(pres, 2);
  Serial.print("  T(C): "); Serial.println(temp, 2);

  delay(500);
}

// void setup() {
//   // put your setup code here, to run once:
//   // pinMode(13, OUTPUT);
//   Serial.begin(115200);
//   delay(2000);

//   Serial.println("Starting MPL3115A2...");

//   if (!mpl.begin()) {
//     Serial.println("Sensor not found");
//     while (1) delay(10);
//   }

//   Serial.println("Sensor found!");

// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   // digitalWrite(13, HIGH);
//   // delay(1000);
//   // digitalWrite(13, LOW);
//   // delay(1000);
//   // exit(0);
//   Serial.print("Altitude: ");
//   Serial.println(mpl.getAltitude());
//   delay(500);
// }


// check to see serial monitor is compatible 
// void setup() {
//   Serial.begin(115200);
//   delay(2000);
//   Serial.println("=== BOOT: setup reached ===");
// }

// void loop() {
//   Serial.println("tick");
//   delay(500);
// }