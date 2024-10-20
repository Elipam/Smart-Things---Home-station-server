#include "DHT.h"
#include <Arduino.h>

const int ldrPin = 36;

DHT dht(0, DHT22);

const int irSensorPin = 25;   // IR sensor pin
int pulseCount = 0;           // Om het aantal "high" waarden bij te houden
unsigned long startTime = 0;  // Begin tijd voor de 10 seconden
int lightValue = 0;

void checkBrightness(){
  lightValue = analogRead(ldrPin);
  Serial.println(lightValue);
  delay(500);
}

void checkTempHum(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
  Serial.println(F("Failed to read from DHT sensor!"));
  }
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.println();
  delay(4000);
}

void countRotation(){
  pulseCount = 0;
  startTime = millis();

  while (millis() - startTime < 10000) {
    if (digitalRead(irSensorPin) == LOW) {
      pulseCount++;
      delay(100);
      Serial.println(pulseCount);

      while (digitalRead(irSensorPin) == LOW) {
      }
      delay(100);
    }
  }

  Serial.print("Aantal pulsen in 10 seconden: ");
  Serial.println(pulseCount);
}

void setup() {
  Serial.begin(9600);
  analogSetAttenuation(ADC_11db);
  pinMode(irSensorPin, INPUT);

  dht.begin();
}

void loop() {
  checkBrightness();
}