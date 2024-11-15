#include "DHT.h"
#include <Arduino.h>

float calculateAverage(float array[], int count) {
  if (count == 0) return 0;
  float total = 0;
  for (int i = 0; i < count; i++) {
    total += array[i];
  }
  return total / count;
}

int RES_pin = 5;
int RCK_pin = 0;
int SCK_pin = 4;
int RES_pin2 = 13;
int RCK_pin2 = 14;
int SCK_pin2 = 12;
int dec_digits[12] = {3, 159, 37, 13, 153, 73, 65, 31, 1, 9, 254, 0};

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
  if (index1 < 0 || index1 >= 12 || index2 < 0 || index2 >= 12) {
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

void weatherCode(int wind, int temp) {

}

const int LIGHT_SENSOR_PIN = A0;
int lux;

void checkBrightness() {
  float measurements[10];
  
  for (int i = 0; i < 10; i++) {
    measurements[i] = analogRead(LIGHT_SENSOR_PIN);
    delay(500);
  }

  int average = calculateAverage(measurements, 10);
  lux = (average  + 434.5) / 2.5226;

  Serial.print("Hoeveelheid licht in lux = ");
  Serial.println(lux);

  // Controleer of de waarde in het bereik van 1000 of meer ligt om deze te splitsen
  if (lux >= 100) {
    // Splits waarde in tweeën: eerst de eerste twee cijfers, daarna de laatste twee
    int firstPart = lux / 100;    // Bijvoorbeeld, 1035 -> 10
    int secondPart = lux % 100;   // Bijvoorbeeld, 1035 -> 35

    // Toon eerst de eerste twee cijfers voor 3 seconden
    showData(firstPart);
    delay(3000);

    // Toon vervolgens de laatste twee cijfers voor 3 seconden
    showData(secondPart);
    delay(3000);
  } else {
    showData(00);
    delay(3000);
    // Als de waarde minder dan 100 is, toon het hele getal voor 5 seconden
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

  float avgTemperature = validMeasurements > 0 ? calculateAverage(temperatureArray, validMeasurements) : -999; // -999 betekent fout

  if (avgTemperature != -999) {
    Serial.print("Temperatuur in °C: ");
    Serial.println(avgTemperature);
    showData(avgTemperature);
    delay(5000);
  } else {
    showDataIndex(11, 11);  // Toont error code
  }

  return avgTemperature;
}

void checkHumidity() {
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

  if (validMeasurements > 0) {
    int avgHumidity = calculateAverage(humidityArray, validMeasurements);
    Serial.print("Luchtvochtigheid in percentage waterdamp: ");
    Serial.println(avgHumidity);
    showData(avgHumidity);
    delay(5000);
  } else {
    showDataIndex(11, 11);  
  }
}

int pulseCount;
const int irSensorPin = 9;
unsigned long startTime = 0;
int windsnelheid;

float checkWindSpeed() {
  int pulseCount = 0;
  unsigned long startTime = millis();
  unsigned long interval = 10000; // 10 seconden interval

  while (millis() - startTime < interval) {
    if (digitalRead(irSensorPin) == LOW) {
      pulseCount++;
      delay(50);  // kort delay om debounce van sensor te voorkomen

      // Wacht totdat de sensor niet meer LOW is om dubbel tellen te voorkomen
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

#define RED_LED_PIN D4 
#define ORANGE_LED_PIN D8
#define GREEN_LED_PIN 1

void weatherLED(int wind, int temp) {
  // Controleer gevaarlijke weersomstandigheden
  if (wind > 20 || temp > 35 || temp < -5) {
    // Rood LED brandt bij gevaarlijke omstandigheden
    digitalWrite(RED_LED_PIN, HIGH);
    Serial.println("Gevaarlijke weersomstandigheden: rood LED aan.");
  }
  // Controleer matige weersomstandigheden
  else if (wind > 10 || temp < 0|| temp > 30) {
    // Oranje LED brandt bij matige omstandigheden
    digitalWrite(ORANGE_LED_PIN, HIGH);
    Serial.println("Matige weersomstandigheden: oranje LED aan.");
  }
  // Goede weersomstandigheden
  else {
    // Groen LED brandt bij ideale omstandigheden
    digitalWrite(GREEN_LED_PIN, HIGH);
    Serial.println("Goede weersomstandigheden: groen LED aan.");
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

  pinMode(LED, OUTPUT); // set the digital pin as output.
  pinMode(LED2, OUTPUT); // set the digital pin as output.
  pinMode(LED3, OUTPUT); // set the digital pin as output.

  // Zet alle LEDs uit
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(ORANGE_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

}

void loop() {
  // Meting 1: Helderheid
  showDataIndex(10, 1);  // Toon '8.1' voor 2 seconden
  checkBrightness();  // Toon gemeten helderheid gedurende 5 seconden
  
  // Meting 2: Temperatuur
  showDataIndex(10, 2);  // Toon '8.2' voor 2 seconden
  checkTemperature();     // Toon gemeten temperatuur gedurende 5 seconden

  // Meting 3: Luchtvochtigheid
  showDataIndex(10, 3);  // Toon '8.3' voor 2 seconden
  checkHumidity();        // Toon gemeten luchtvochtigheid gedurende 5 seconden

  // Meting 4: Windsnelheid
  showDataIndex(10, 4);  // Toon '8.4' voor 2 seconden
  checkWindSpeed();       // Toon gemeten windsnelheid gedurende 5 seconden

  weatherLED(windsnelheid, avgTemperature);
}