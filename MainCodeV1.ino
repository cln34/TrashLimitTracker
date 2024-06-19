
//#include <WiFi101.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <ESP8266WebServer.h>



#include "HX711.h"
#include <Adafruit_NeoPixel.h>

//Messwerte für Nachricht wenn Mülleimer voll ist
bool muelleimerVoll = false;
int redStartTime = 0;
bool isRed = false;

const char *ssid     = "Obi Wlan Kenobi";
const char *password = "Ichstehedeutlichueberdir";

const long utcOffsetInSeconds = 3600;


WiFiUDP ntpUDP;
WiFiClient wifiClient;
ESP8266WebServer server(80);


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

WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Ich verbinde mich mit dem Internet...");
  }

  
  
  Serial.println("Ich bin mit dem Internet verbunden!");

 // Starten Sie den Webserver
  
  server.on("/page0", handleRoot0);
  server.on("/page1", handleRoot1);
  server.on("/page2", handleRoot2);
  server.on("/page3", handleRoot3);

  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  server.handleClient(); // Bearbeiten Sie eingehende HTTP-Anfragen
  
  //Waage
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
    displayPage0 = false;
    displayPage1 = false;
    displayPage2 = true;
    displayPage3 = false;
  } else if (distance > 23 && messwertfinal < 500) {
    color = pixels.Color(0, 255, 0); // Grün für lange Distanz & geringes Gewicht
    isRed = false;
    displayPage0 = false;
    displayPage1 = true;
    displayPage2 = false;
    displayPage3 = false;
  } else if (distance > 23 && messwertfinal >= 501 && messwertfinal < 1000) {
    color = pixels.Color(0, 255, 0); // Grün für lange Distanz & mittleres Gewicht
    isRed = false;
    displayPage0 = false;
    displayPage1 = true;
    displayPage2 = false;
    displayPage3 = false;
  } else {
    color = pixels.Color(255, 255, 0); // Gelb für lange Distanz & hohes Gewicht
    isRed = false;
    displayPage0 = false;
    displayPage1 = false;
    displayPage2 = true;
    displayPage3 = false;
  }

  setColor(color);

  // Wenn LEDs länger als 20 sek rot leuchten, dann ist der mülleimer voll
  if (isRed && (millis() - redStartTime > 20000)) {
    
    Serial.println("Mülleimer ist voll");
    displayPage0 = false;
    displayPage1 = false;
    displayPage2 = false;
    displayPage3 = true;
  }


//Website 

if (displayPage0) {
    server.sendHeader("Location", "/page0");
    server.send(302, "text/plain", "");
  }

  if (displayPage1) {
    server.sendHeader("Location", "/page1");
    server.send(302, "text/plain", "");
  }

  if (displayPage2) {
    server.sendHeader("Location", "/page2");
    server.send(302, "text/plain", "");
  }
  
  if (displayPage3) {
    server.sendHeader("Location", "/page3");
    server.send(302, "text/plain", "");
  }



  delay(1000);
}

void handleRoot0() {
  String html = "<!DOCTYPE html>"
                "<html lang=\"de\">"
                "<head>"
                "<meta charset=\"UTF-8\">"
                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
                "<title>Zustand: Leer</title>"
                "<style>"
                "body {"
                "  margin: 0;"
                "  padding: 0;"
                "  font-family: Arial, sans-serif;"
                "  background-color: #000000;"
                "  color: white;"
                "  display: flex;"
                "  flex-direction: column;"
                "  justify-content: center;"
                "  align-items: center;"
                "  height: 100vh;"
                "  text-align: center;"
                "}"
                ".container {"
                "  display: flex;"
                "  flex-direction: column;"
                "  align-items: center;"
                "}"
                ".icon {"
                "  width: 100px;"
                "  height: 100px;"
                "  background-color: white;"
                "  border-radius: 50%;"
                "  display: flex;"
                "  justify-content: center;"
                "  align-items: center;"
                "  color: #2ecc71;"
                "  font-size: 2em;"
                "  margin-bottom: 20px;"
                "}"
                "h1 {"
                "  margin: 0;"
                "}"
                ".logo {"
                "  font-size: 1.5em;"
                "  font-weight: bold;"
                "  margin-bottom: 20px;"
                "}"
                ".footer {"
                "  position: absolute;"
                "  bottom: 10px;"
                "  font-size: 0.8em;"
                "}"
                "</style>"
                "</head>"
                "<body>"
                "<div class=\"container\">"
                "<div class=\"logo\">TrashLimitTracker</div>"
                "<div class=\"icon\">✅</div>"
                "<h1>Zustand: Leer</h1>"
                "<p>Der Behälter ist leer und bereit zur Nutzung.</p>"
                "</div>"
                "<div class=\"footer\">© 2024 TrashLimitTracker</div>"
                "<script>"
                "setTimeout(function() { location.reload(); }, 1000);" // Aktualisiert die Seite alle 5 Sekunden
                "</script>"
                "</body>"
                "</html>";

  server.send(200, "text/html", html);
}

void handleRoot1() {
  String html = "<!DOCTYPE html>"
                "<html lang=\"de\">"
                "<head>"
                "<meta charset=\"UTF-8\">"
                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
                "<title>Zustand: Leer</title>"
                "<style>"
                "body {"
                "  margin: 0;"
                "  padding: 0;"
                "  font-family: Arial, sans-serif;"
                "  background-color: #2ecc71;"
                "  color: white;"
                "  display: flex;"
                "  flex-direction: column;"
                "  justify-content: center;"
                "  align-items: center;"
                "  height: 100vh;"
                "  text-align: center;"
                "}"
                ".container {"
                "  display: flex;"
                "  flex-direction: column;"
                "  align-items: center;"
                "}"
                ".icon {"
                "  width: 100px;"
                "  height: 100px;"
                "  background-color: white;"
                "  border-radius: 50%;"
                "  display: flex;"
                "  justify-content: center;"
                "  align-items: center;"
                "  color: #2ecc71;"
                "  font-size: 2em;"
                "  margin-bottom: 20px;"
                "}"
                "h1 {"
                "  margin: 0;"
                "}"
                ".logo {"
                "  font-size: 1.5em;"
                "  font-weight: bold;"
                "  margin-bottom: 20px;"
                "}"
                ".footer {"
                "  position: absolute;"
                "  bottom: 10px;"
                "  font-size: 0.8em;"
                "}"
                "</style>"
                "</head>"
                "<body>"
                "<div class=\"container\">"
                "<div class=\"logo\">TrashLimitTracker</div>"
                "<div class=\"icon\">✅</div>"
                "<h1>Zustand: Leer</h1>"
                "<p>Der Behälter ist leer und bereit zur Nutzung.</p>"
                "</div>"
                "<div class=\"footer\">© 2024 TrashLimitTracker</div>"
                "<script>"
                "setTimeout(function() { location.reload(); }, 1000);" // Aktualisiert die Seite alle 5 Sekunden
                "</script>"
                "</body>"
                "</html>";

  server.send(200, "text/html", html);
}

void handleRoot2() {
  String html = "<!DOCTYPE html>"
                "<html lang=\"de\">"
                "<head>"
                "<meta charset=\"UTF-8\">"
                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
                "<title>Zustand: Mittelvoll</title>"
                "<style>"
                "body {"
                "  margin: 0;"
                "  padding: 0;"
                "  font-family: Arial, sans-serif;"
                "  background-color: #f1c40f;"
                "  color: white;"
                "  display: flex;"
                "  flex-direction: column;"
                "  justify-content: center;"
                "  align-items: center;"
                "  height: 100vh;"
                "  text-align: center;"
                "}"
                ".container {"
                "  display: flex;"
                "  flex-direction: column;"
                "  align-items: center;"
                "}"
                ".icon {"
                "  width: 100px;"
                "  height: 100px;"
                "  background-color: white;"
                "  border-radius: 50%;"
                "  display: flex;"
                "  justify-content: center;"
                "  align-items: center;"
                "  color: #f1c40f;"
                "  font-size: 2em;"
                "  margin-bottom: 20px;"
                "}"
                "h1 {"
                "  margin: 0;"
                "}"
                ".logo {"
                "  font-size: 1.5em;"
                "  font-weight: bold;"
                "  margin-bottom: 20px;"
                "}"
                ".footer {"
                "  position: absolute;"
                "  bottom: 10px;"
                "  font-size: 0.8em;"
                "}"
                "</style>"
                "</head>"
                "<body>"
                "<div class=\"container\">"
                "<div class=\"logo\">TrashLimitTracker</div>"
                "<div class=\"icon\">⚠️</div>"
                "<h1>Zustand: Mittelvoll</h1>"
                "<p>Der Behälter ist mittelvoll. Überprüfen Sie den Füllstand regelmäßig.</p>"
                "</div>"
                "<div class=\"footer\">© 2024 TrashLimitTracker</div>"
                "<script>"
                "setTimeout(function() { location.reload(); }, 1000);" // Aktualisiert die Seite alle 5 Sekunden
                "</script>"
                "</body>"
                "</html>";

  server.send(200, "text/html", html);
}

void handleRoot3() {
  String html = "<!DOCTYPE html>"
                "<html lang=\"de\">"
                "<head>"
                "<meta charset=\"UTF-8\">"
                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
                "<title>Zustand: Voll</title>"
                "<style>"
                "body {"
                "  margin: 0;"
                "  padding: 0;"
                "  font-family: Arial, sans-serif;"
                "  background-color: #e74c3c;"
                "  color: white;"
                "  display: flex;"
                "  flex-direction: column;"
                "  justify-content: center;"
                "  align-items: center;"
                "  height: 100vh;"
                "  text-align: center;"
                "}"
                ".container {"
                "  display: flex;"
                "  flex-direction: column;"
                "  align-items: center;"
                "}"
                ".icon {"
                "  width: 100px;"
                "  height: 100px;"
                "  background-color: white;"
                "  border-radius: 50%;"
                "  display: flex;"
                "  justify-content: center;"
                "  align-items: center;"
                "  color: #e74c3c;"
                "  font-size: 2em;"
                "  margin-bottom: 20px;"
                "}"
                "h1 {"
                "  margin: 0;"
                "}"
                ".logo {"
                "  font-size: 1.5em;"
                "  font-weight: bold;"
                "  margin-bottom: 20px;"
                "}"
                ".footer {"
                "  position: absolute;"
                "  bottom: 10px;"
                "  font-size: 0.8em;"
                "}"
                "</style>"
                "</head>"
                "<body>"
                "<div class=\"container\">"
                "<div class=\"logo\">TrashLimitTracker</div>"
                "<div class=\"icon\">⚠️</div>"
                "<h1>Zustand: Voll</h1>"
                "<p>Der Behälter ist voll und muss dringend geleert werden!</p>"
                "</div>"
                "<div class=\"footer\">© 2024 TrashLimitTracker</div>"
                "<script>"
                "setTimeout(function() { location.reload(); }, 1000);" // Aktualisiert die Seite alle 5 Sekunden
                "</script>"
                "</body>"
                "</html>";

  server.send(200, "text/html", html);
}

void setColor(uint32_t color) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show(); // Aktualisiert die LEDs mit der neuen Farbe
}


