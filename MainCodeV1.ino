

#include "HX711.h"
#include <Adafruit_NeoPixel.h>


// HX711 Verdrahtung
#define WAAGE_1_DOUT_PIN  D5 // DT Pin des Hx711
#define WAAGE_1_SCK_PIN  D6  // SCK Pin des Hx711

HX711 waage1;

//long double messwert0;
//long double messwert1;
//long raw 10004656000000L;
float messwertfinal;

//Neopixel Verdrahtung
#define PIN D8 // datenpin neopixel
#define NUMPIXELS 8 // anzahl LEDs neopixel

//Ultraschall
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
    //long double raw  = 10004656000000;
    //long double messwert0 = ((waage1.get_units(10)) + 1000000000);
    //long double messwert1 =((raw/messwert0 )*100000 )/D2 /111;
    float messwert0 =((waage1.read() * -0.0001)+46.7);

    messwertfinal = (((messwert0/0.1)-1)*44)+55;
    
    
    
    
    
    Serial.print("Messstelle 1 Wert: ");
    //Serial.println(messwert0, 5);
    Serial.println(messwertfinal, 5);
    Serial.println();
    Serial.println();
  }
  else {
    Serial.println("HX711 Messstelle 1 nicht verbunden.");
  }

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
  // Rot für kurze Distanz
  if (distance < 15) {
    setColor(pixels.Color(255, 0, 0)); 

  // Gelb für mittlere Distanz
  } else if (distance >= 15 && distance < 23) {
    setColor(pixels.Color(255, 255, 0)); 
  } 
  
  //Fälle nach Gewicht und langer Distanz
  //Fall 1: Lange Distanz & geringes Gewicht (grün)
  else if (distance > 23 && messwertfinal < 500){
     setColor(pixels.Color(0, 255, 0)); 
  } 

  //Fall 2: Lange Distanz & mittleres Gewicht (grün)
  else if(distance > 23 && messwertfinal >= 501 && messwertfinal < 1000){
    setColor(pixels.Color(0, 255, 0)); 
  }

  //Fall 3: Lange Distanz & hohes Gewicht (gelb)
  else{
    setColor(pixels.Color(255, 255, 0)); 
  }

  ;


  delay(1000);

}


void setColor(uint32_t color) {
  for(int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
    
  }
  pixels.show(); // Aktualisiert die LEDs mit der neuen Farbe
}