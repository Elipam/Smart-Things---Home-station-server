#include "DHT.h"
#include <Arduino.h>

// Functie om gemiddelde te berekenen
float calculateAverage(float array[], int count) {
  if (count == 0) return 0;
  float total = 0;
  for (int i = 0; i < count; i++) {
    total += array[i];
  }
  return total / count;
}

// Pin-definities voor display
int RES_pin = 5;
int RCK_pin = 0;
int SCK_pin = 4;
int RES_pin2 = 13;
int RCK_pin2 = 14;
int SCK_pin2 = 12;
int dec_digits[13] = {3, 159, 37, 13, 153, 73, 65, 31, 1, 9, 254, 0, 255};

// Functies om data op het display te tonen
void showData(int data) {
  int tens = (data / 10) % 10;
  int ones = data % 10;

  digitalWrite(RCK_pin, LOW);
  shiftOut(RES_pin, SCK_pin, LSBFIRST, dec_digits[tens]);
  digitalWrite(RCK_pin, HIGH);

  digitalWrite(RCK_pin2, LOW);
  shiftOut(RES_pin2, SCK_pin2, LSBFIRST, dec_digits[ones]);
  digitalWrite(RCK_pin2, HIGH);
}

void showDataIndex(int index1, int index2) {
  if (index1 < 0 || index1 >= 13 || index2 < 0 || index2 >= 13) {
    Serial.println("Ongeldige index in dec_digits.");
    return;
  }

  digitalWrite(RCK_pin, LOW);
  shiftOut(RES_pin, SCK_pin, LSBFIRST, dec_digits[index1]);
  digitalWrite(RCK_pin, HIGH);

  digitalWrite(RCK_pin2, LOW);
  shiftOut(RES_pin2, SCK_pin2, LSBFIRST, dec_digits[index2]);
  digitalWrite(RCK_pin2, HIGH);
}

// Functies om weerdata te meten
const int LIGHT_SENSOR_PIN = A0;
int lux;

void checkBrightness() {
  float measurements[10];
  
  for (int i = 0; i < 10; i++) {
    measurements[i] = analogRead(LIGHT_SENSOR_PIN);
    delay(500);
  }

  int average = calculateAverage(measurements, 10);
  lux = (average + 434.5) / 2.5226;

  Serial.print("Hoeveelheid licht in lux = ");
  Serial.println(lux);

  if (lux >= 100) {
    int firstPart = lux / 100;
    int secondPart = lux % 100;
    showData(firstPart);
    delay(3000);
    showData(secondPart);
    delay(3000);
  } else {
    showData(00);
    delay(3000);
    showData(lux);
    delay(3000);
  }
}

#define DHTPIN 10         
#define DHTTYPE DHT11    

DHT dht(DHTPIN, DHTTYPE);

float checkTemperature() {
  float temperatureArray[10];
  int validMeasurements = 0;

  for (int i = 0; i < 10; i++) {
    float temperature = dht.readTemperature();

    if (!isnan(temperature)) {
      temperatureArray[validMeasurements] = temperature;
      validMeasurements++;
    }
    delay(500);
  }

  float avgTemperature = validMeasurements > 0 ? calculateAverage(temperatureArray, validMeasurements) : -999;

  if (avgTemperature != -999) {
    Serial.print("Temperatuur in °C: ");
    Serial.println(avgTemperature);
    showData(avgTemperature);
    delay(5000);
  } else {
    showDataIndex(11, 11);
  }

  return avgTemperature;
}

float checkHumidity() {
  float humidityArray[10];
  int validMeasurements = 0;

  for (int i = 0; i < 10; i++) {
    float humidity = dht.readHumidity();

    if (!isnan(humidity)) {
      humidityArray[validMeasurements] = humidity;
      validMeasurements++;
    }
    delay(500);
  }

  float avgHumidity = validMeasurements > 0 ? calculateAverage(humidityArray, validMeasurements) : -999;

  if (avgHumidity != -999) {
    Serial.print("Luchtvochtigheid in percentage waterdamp: ");
    Serial.println(avgHumidity);
    showData(avgHumidity);
    delay(5000);
  } else {
    showDataIndex(11, 11);  
  }

  return avgHumidity;
}

const int irSensorPin = 9;

float checkWindSpeed() {
  int pulseCount = 0;
  unsigned long startTime = millis();
  unsigned long interval = 10000; // 10 seconden interval

  while (millis() - startTime < interval) {
    if (digitalRead(irSensorPin) == LOW) {
      pulseCount++;
      delay(50);

      while (digitalRead(irSensorPin) == LOW) {
        delay(1);
      }
    }
    delay(10);
  }

  float windsnelheid = (pulseCount + 3.0873) / 3.7129;
  Serial.print("Windsnelheid in m/s: ");
  Serial.println(windsnelheid);

  showData(windsnelheid);
  delay(5000);

  return windsnelheid;
}

// LED-configuratie
#define RED_LED_PIN D4 
#define ORANGE_LED_PIN D8
#define GREEN_LED_PIN 1

void weatherLED(float wind, float temp) {
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(ORANGE_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  if (wind > 20 || temp > 35 || temp < -5) {
    digitalWrite(RED_LED_PIN, HIGH);
    delay(4000);
    Serial.println("Gevaarlijke weersomstandigheden: rood LED aan.");
    digitalWrite(RED_LED_PIN, LOW);
  } else if (wind > 10 || (temp > 30 && temp < 0)) {
    digitalWrite(ORANGE_LED_PIN, HIGH);
    delay(4000);
    Serial.println("Matige weersomstandigheden: oranje LED aan.");
    digitalWrite(ORANGE_LED_PIN, LOW);
  } else {
    digitalWrite(GREEN_LED_PIN, HIGH);
    delay(4000);
    Serial.println("Goede weersomstandigheden: groen LED aan.");
    digitalWrite(GREEN_LED_PIN, LOW);
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(RES_pin, OUTPUT);
  pinMode(RCK_pin, OUTPUT);
  pinMode(SCK_pin, OUTPUT);
  pinMode(RES_pin2, OUTPUT);
  pinMode(RCK_pin2, OUTPUT);
  pinMode(SCK_pin2, OUTPUT); 

  dht.begin();

  pinMode(irSensorPin, INPUT);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(ORANGE_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(ORANGE_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
}

void loop() {
  showDataIndex(10, 1);  
  checkBrightness();     

  showDataIndex(10, 2);   
  float avgTemperature = checkTemperature();

  showDataIndex(10, 3);   
  checkHumidity();

  showDataIndex(10, 4);   
  float windsnelheid = checkWindSpeed();     

  weatherLED(windsnelheid, avgTemperature);  

  showDataIndex(12, 12);
  delay(3000);
}
