#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

#include <ArduinoJson.h>

#include "ESP8266WiFi.h"
#include <Wire.h>  
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "yourssid";
const char* password = "yourpassword";

const char* host = "services1.arcgis.com";
String request = "/0MSEUqKaxRlEPj5g/arcgis/rest/services/Coronavirus_2019_nCoV_Cases/FeatureServer/1/query?where=UPPER(Country_Region)%20like%20%27%25INDIA%25%27&outFields=Country_Region,Long_,Confirmed,Recovered,Deaths,Last_Update&returnGeometry=false&outSR=4326&f=json";
const int httpsPort = 443;
const char fingerprint[] PROGMEM = "70:58:0E:78:0C:9D:72:75:50:61:9D:3E:4E:FD:B2:1D:64:D1:E9:1E"; //SHA1 finger print

void setup() {
  delay(100);
  lcd.init();   // initializing the LCD
  lcd.backlight();
    
  Serial.begin(115200);
  Serial.print("Connecting");
  lcd.setCursor(0, 0);
  lcd.print("Connecting");
  
  

  delay(1000);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue
  delay(1000);
  WiFi.mode(WIFI_STA);        //Station mode
  WiFi.begin(ssid, password);     //Connect to WiFi

   while (WiFi.status() != WL_CONNECTED) {  // Wait for connection
    delay(500);
    //lcd.setCursor(0, 1);
    Serial.print(".");
    lcd.print(".");
  }

  Serial.println("");  //If connection successful show IP address of ESP8266 in serial monitor
  Serial.print("Connected to ");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WIFI Connected");
  lcd.setCursor(0, 1);
  lcd.print("to:");
  lcd.print(ssid);
  
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

  Serial.println(host);
  Serial.printf("Using fingerprint '%s'\n", fingerprint);

  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(15000); // 15 Seconds
  delay(1000);

  Serial.println("HTTPS Connecting");

  int r = 0; //retry counter
  while ((!httpsClient.connect(host, httpsPort)) && (r < 30)) {
    delay(100);
    Serial.print(".");
    r++;
  }
  
  if (r == 30) {
    Serial.println("Connection failed");
  }
  else {
    Serial.println("Connected");
  }

  Serial.print("Requesting: ");
  Serial.println(host + request);

  httpsClient.print(String("GET ") + request + " HTTP/1.1\r\n" +
                    "Host: " + host + "\r\n" +
                    "Connection: close\r\n\r\n");

  Serial.println("Request sent");

  while (httpsClient.connected()) {
    String line = httpsClient.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("Headers received");
      break;
    }
  }

  Serial.println("Payload received:");

  String payload;
  while (httpsClient.available()) {
    payload = httpsClient.readStringUntil('\n');  //Read Line by Line
    //Serial.println(payload); //Print response
  }

  Serial.println("Closing connection");

  char charBuf[500];
  payload.toCharArray(charBuf, 500);
Serial.println(charBuf);
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(6) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + 5*JSON_OBJECT_SIZE(6) + 3*JSON_OBJECT_SIZE(7) + 940;
DynamicJsonDocument doc(capacity);

//const char* json = charBuf;

deserializeJson(doc, payload);

JsonObject features_0_attributes = doc["features"][0]["attributes"];
const char* features_0_attributes_Country_Region = features_0_attributes["Country_Region"]; // "India"
float features_0_attributes_Long_ = features_0_attributes["Long_"]; // 78.96288
int features_0_attributes_Confirmed = features_0_attributes["Confirmed"]; // Confired cases
int features_0_attributes_Recovered = features_0_attributes["Recovered"]; // recovered cases
int features_0_attributes_Deaths = features_0_attributes["Deaths"]; // deaths
long features_0_attributes_Last_Update = features_0_attributes["Last_Update"]; // 1585646307000

  Serial.println(features_0_attributes_Confirmed);
  Serial.println(features_0_attributes_Deaths);
  Serial.println(features_0_attributes_Recovered);
lcd.backlight();
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("INDIA Conf: ");lcd.print(features_0_attributes_Confirmed);
lcd.setCursor(0, 1);
lcd.print("D:");lcd.print(features_0_attributes_Deaths);
lcd.print(" R:");lcd.print(features_0_attributes_Recovered);
 delay(3000);
 lcd.noBacklight();
  delay(60000);
}
