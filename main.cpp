#include <ESP8266WiFi.h> // ESP8266 library
#include <AdafruitIO.h>  // Adafruit IO library
#include <Adafruit_MQTT.h> // Adafruit MQTT library
#include <ArduinoHttpClient.h>  // http Client
#include <HX711.h>  // HX711 library for the scla
#include "DFRobot_HT1632C.h" // Library for DFRobot LED matrix display
#include "WiFi.h"
#include "HTTPSRedirect.h"


#define calibration_factor -21700.0 

#define DOUT 0  // Pin connected to HX711 data output pin
#define CLK  12  // Pin connected to HX711 clk pin

// WiFi credentials
const char* ssid = "BTWholeHome-Z7NK";         // change SSID
const char* password = "December27";    // change password

// Gscript ID and required credentials
const char *GScriptId = "https://script.google.com/macros/s/AKfycbzQ0JBi6rqXjSqw2WzeWeyMEsWFMfwsKqS-5bhGMa9-MIOcVhSb/exec";    // change Gscript ID
const int httpsPort =  443;
const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";
String url = String("/macros/s/") + GScriptId + "/exec?";
HTTPSRedirect client(httpsPort);

void startWiFi(){
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" IP address: ");
  Serial.println(WiFi.localIP());

  
  Serial.print(String("Connecting to "));
  Serial.println(host);

  bool WiFiFlag = false;
  for (int i=0; i<5; i++){
    int retval = client.connect(host, httpsPort);
    if (retval == 1) {
       WiFiFlag = true;
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  // Connection Status, 1 = Connected, 0 is not.
  Serial.println("Connection Status: " + String(client.connected()));
  Serial.flush();
  
  if (!WiFiFlag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    Serial.flush();
    return;
  }
}

HX711 scale(DOUT, CLK);

float weight = 0.0;
float prev_weight = 0.0;

void setup() {
  Serial.begin(115200);
  startWiFi();
  scale.set_scale(calibration_factor); 
  scale.tare(); 
}

void loop() {
 
  weight = scale.get_units();
  Serial.println(weight);
  postData(weight);
  
}