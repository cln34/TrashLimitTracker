#include "HX711.h"
#include <Adafruit_NeoPixel.h>

//Messwert für Nachricht
bool muelleimerVoll = false;
int redStartTime = 0;
bool isRed = false;


// HX711 Verdrahtung
#define WAAGE_1_DOUT_PIN  D5 // DT Pin des Hx711
#define WAAGE_1_SCK_PIN  D6  // SCK Pin des Hx711

HX711 waage1;

// Gewichtssensor
float messwertfinal;

// Neopixel Verdrahtung
#define PIN D8 // datenpin neopixel
#define NUMPIXELS 8 // anzahl LEDs neopixel

// Ultraschall
const int trigPin = D1;
const int echoPin = D2;
long duration;
int distance;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);



void setup() {
  Serial.begin(115200);
  Serial.println("HX711 Test");
  waage1.begin(WAAGE_1_DOUT_PIN, WAAGE_1_SCK_PIN);
 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pixels.begin();
  pixels.setBrightness(8);
}

void loop() {
  if (waage1.is_ready()) {
    float messwert0 = ((waage1.read() * -0.0001) + 46.7);
    messwertfinal = (((messwert0 / 0.1) - 1) * 44) + 55;
    Serial.print("Messstelle 1 Wert: ");
    Serial.println(messwertfinal, 5);
    Serial.println();
  } else {
    Serial.println("HX711 Messstelle 1 nicht verbunden.");
  }

  // TrigPin auf low setzen
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // TrigPin auf high setzen für 10 Mikrosekunden
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Echo Pin lesen und die Zeitdauer messen
  duration = pulseIn(echoPin, HIGH);

  // Abstand berechnen
  distance = duration * 0.034 / 2;

  // LEDs basierend auf dem Abstand steuern
  uint32_t color;
  if (distance < 15) {
    color = pixels.Color(255, 0, 0); // Rot für kurze Distanz
    if (!isRed) {
      isRed = true;
      redStartTime = millis(); // Start the timer
    }
  } else if (distance >= 15 && distance < 23) {
    color = pixels.Color(255, 255, 0); // Gelb für mittlere Distanz
    isRed = false;
  } else if (distance > 23 && messwertfinal < 500) {
    color = pixels.Color(0, 255, 0); // Grün für lange Distanz & geringes Gewicht
    isRed = false;
  } else if (distance > 23 && messwertfinal >= 501 && messwertfinal < 1000) {
    color = pixels.Color(0, 255, 0); // Grün für lange Distanz & mittleres Gewicht
    isRed = false;
  } else {
    color = pixels.Color(255, 255, 0); // Gelb für lange Distanz & hohes Gewicht
    isRed = false;
  }

  setColor(color);

  // Wenn LEDs länger als 20 sek rot leuchten, dann ist der mülleimer voll
  if (isRed && (millis() - redStartTime > 20000)) {
    muelleimerVoll = true;
    Serial.println("Mülleimer ist voll");
  }
 

  delay(1000);
}

void setColor(uint32_t color) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show(); // Aktualisiert die LEDs mit der neuen Farbe
}
