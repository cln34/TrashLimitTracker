#include <Adafruit_NeoPixel.h>

#define PIN 6 // datenpin neopixel
#define NUMPIXELS 8 // anzahl LEDs neopixel

const int trigPin = 9;
const int echoPin = 10;
long duration;
int distance;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pixels.begin();
  Serial.begin(9600); 
}

void loop() {
  // trigpin auf low setzen
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // trigpin auf high setzen für 10 mikrosekunden
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // echo pin lesen und die zeitdauer messen
  duration = pulseIn(echoPin, HIGH);

  // abstand berechnen
  distance = duration * 0.034 / 2;

  // LEDs basierend auf dem abstand steuern
  if (distance < 100) {
    setColor(pixels.Color(255, 0, 0)); // Rot für kurze Distanz
  } else if (distance >= 100 && distance < 200) {
    setColor(pixels.Color(255, 255, 0)); // Gelb für mittlere Distanz
  } else{
    setColor(pixels.Color(0, 255, 0)); // Grün für lange Distanz
  } /*
    setColor(pixels.Color(0, 0, 255)); // Blau für Fehlerzustand
  */

  ;
}

void setColor(uint32_t color) {
  for(int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show(); // Aktualisiert die LEDs mit der neuen Farbe
}
