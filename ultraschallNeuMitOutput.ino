#include <Adafruit_NeoPixel.h>

#define PIN 8 // datenpin neopixel
#define NUMPIXELS 8 // anzahl LEDs neopixel

const int trigPin = 10;
const int echoPin = 9;
long duration;
int distance;
bool printedRed = false;
bool printedGreen = false;
bool printedYellow = false;

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

  if (distance < 10) {
    printedYellow = false;
    printedGreen = false;
    setColor(pixels.Color(255, 0, 0)); // Rot für kurze Distanz
    if(!printedRed){
      Serial.println("Rot");
      printedRed = true;
    }
  } else if (distance >= 10 && distance < 20) {
    printedRed = false;
  printedGreen = false;
    setColor(pixels.Color(255, 255, 0)); // Gelb für mittlere Distanz
    if(!printedYellow){
      Serial.println("Gelb");
      printedYellow = true;
    }
  } else{
    printedRed = false;
    printedYellow = false;
    setColor(pixels.Color(0, 255, 0)); // Grün für lange Distanz
    if(!printedGreen){
      Serial.println("Gruen");
      printedGreen = true;
    }
  }
}
void setColor(uint32_t color) {
  for(int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show(); // Aktualisiert die LEDs mit der neuen Farbe
}
