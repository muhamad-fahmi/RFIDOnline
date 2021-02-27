#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <RFID.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define SS_PIN D4
#define RST_PIN D3

WiFiClient client;
const char* ssid = "SNCO";
const char* password = "setiawan76";

//server host
const char* host = "192.168.1.5";
//server port
const int httpPort = 80;
String key = "YXBsaWthc2lhYnNlbmJ5ZmFobWkwODEyMjQ3NTI5OTk=";
String hostAdd = "http://192.168.1.5:8000/simpankartu";
String hostAbsensi = "http://192.168.1.5:8000/absensi";
String hostMode = "http://192.168.1.5:8000/devicemode/YXBsaWthc2lhYnNlbmJ5ZmFobWkwODEyMjQ3NTI5OTk===";
String ModeAlat = "";

RFID rfid(SS_PIN, RST_PIN);



void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.init();


  lcd.begin();
  lcd.backlight();
  lcd.setCursor(2, 1);
  //lcd.clear();
  lcd.print("PROGRAM STARTED!!!");
  delay(1000);
  lcd.clear();

  Serial.println("Connecting to :");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting ");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" ");
  Serial.println("WIFI CONNECTED!!!");
 
  ModeDevice();
}

//MODE DEVICE SELECTION

void ModeDevice(){
  HTTPClient http;

  Serial.print("Request Link:");
  Serial.println(hostMode);
  
  http.begin(hostMode);
  
  int httpCode = http.GET();            //Send the request
  String payload = http.getString();    //Get the response payload from server

  Serial.print("Response Code:"); //200 is OK
  Serial.println(httpCode);       //Print HTTP return code

  Serial.print("Returned data from Server:");
  Serial.println(payload);    //Print request response payload
  
  if(httpCode == 200)
  {
    DynamicJsonDocument doc(1024);
  
   // Parse JSON object
    auto error = deserializeJson(doc, payload);
    if (error) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return;
    }
  
    // Decode JSON/Extract values
    String responStatus = doc["status"].as<String>();
    String responMode = doc["mode"].as<String>();
    String responKet = doc["ket"].as<String>();

    Serial.println();
    Serial.print("status : ");
    Serial.println(responStatus);
    
    Serial.print("mode : ");
    Serial.println(responMode);
    
    Serial.print("ket : ");
    Serial.println(responKet);
    Serial.println("-------------------");
    Serial.println();

    lcd.clear();
   // lcd.print("System Absensi RFID");
    if (responMode == "SCAN"){
      ModeAlat = "SCAN";
     
    }else if (responMode == "ADD"){
      ModeAlat = "ADD";
      
    }else{
      ModeAlat = "";
      lcd.setCursor(0,2);
      lcd.print(responKet);
    }
  }
  else
  {
    Serial.println("Error in response");
    lcd.setCursor(3,1);
    lcd.print("SERVER OFFLINE");
  }

  http.end();

  delay(100);
}

//END MODE DEVICE



void loop() {
  if (ModeAlat == "SCAN") {
   
    lcd.setCursor(4, 1);
    lcd.print("WAITING FOR");
    lcd.setCursor(5, 2);
    lcd.print("CARD TAP!");
    if (rfid.isCard()) {
      if (rfid.readCardSerial()) {
        
        lcd.clear();
        Serial.println("");
        Serial.println("Card found");
        String RFID = String(rfid.serNum[0], HEX) + "-" + String(rfid.serNum[1], HEX) + "-" + String(rfid.serNum[2], HEX) + "-" + String(rfid.serNum[3], HEX) + "-" + String(rfid.serNum[4], HEX);
       
       
         String key = "YXBsaWthc2lhYnNlbmJ5ZmFobWkwODEyMjQ3NTI5OTk8899115765756====";
         String host = hostAbsensi;
         host += '/';
         host += RFID;
         host += '/';
         host += key;

          HTTPClient http;

          Serial.print("Request Link:");
          Serial.println(host);
          
          http.begin(host);
          
          int httpCode = http.GET();            //Send the GET request
          String payload = http.getString();    //Get the response payload from server
        
          Serial.print("Response Code:"); //200 is OK
          Serial.println(httpCode);       //Print HTTP return code
        
          Serial.print("Returned data from Server:");
          Serial.println(payload);    //Print request response payload
          
          if(httpCode == 200)
          {
            DynamicJsonDocument doc(1024);
          
           // Parse JSON object
            auto error = deserializeJson(doc, payload);
            if (error) {
              Serial.print(F("deserializeJson() failed with code "));
              Serial.println(error.c_str());
              return;
            }
          
            // Decode JSON/Extract values
            String responStatus = doc["status"].as<String>();
            String responKet = doc["ket"].as<String>();
            String respontap = doc["timetap"].as<String>();
            String responentrytime = doc["entrytime"].as<String>();
            String responlatetime = doc["latetime"].as<String>();
            String respontimeout = doc["timeout"].as<String>();
            int responlate = doc["late"].as<int>();

            lcd.setCursor(2,0);
            lcd.print("UID : ");
            lcd.setCursor(2,1);
            lcd.print(RFID);
            lcd.setCursor(2,2);
            lcd.print("Status : ");
            lcd.setCursor(2,3);
            lcd.print(responKet);
            
            delay(3000);
            lcd.clear();

            
            if(responKet == "UNREGISTERED"){
              lcd.setCursor(3,0);
              lcd.print("PLEASE CONTACT");
              lcd.setCursor(1,1);
              lcd.print("YOUR ADMINISTRATOR");
              lcd.setCursor(4,2);
              lcd.print("TO COMPLETE ");
              lcd.setCursor(4,3);
              lcd.print("OR REGISTER");
              delay(5000);
              lcd.clear();
            }else{
              lcd.setCursor(1,0);
              lcd.print("YOUR TAP : " + respontap);
              lcd.setCursor(1,1);
              lcd.print("ENTRY TIME : " + responentrytime);
              lcd.setCursor(1,2);
              lcd.print("TIME OUT : " + respontimeout);
              lcd.setCursor(1,3);
              if(responlate == 1){
                lcd.print("STATUS : LATE");
              }else{
                lcd.print("STATUS : ON TIME");
              }
  
              delay(5000);
              lcd.clear();

            }

            
            Serial.println();
            Serial.print("status : ");
            Serial.println(responStatus);
            
            Serial.print("ket : ");
            Serial.println(responKet);
            Serial.println("-------------------");
            Serial.println();

        }
      }

    }  
  } else if (ModeAlat == "ADD") {
    lcd.setCursor(4, 1);
    lcd.print("WAITING FOR");
    lcd.setCursor(5, 2);
    lcd.print("NEW CARD!");
    if (rfid.isCard()) {
      if (rfid.readCardSerial()) {
        delay(500);
        lcd.clear();
        Serial.println("");
        Serial.println("Card found");
        String RFID = String(rfid.serNum[0], HEX) + "-" + String(rfid.serNum[1], HEX) + "-" + String(rfid.serNum[2], HEX) + "-" + String(rfid.serNum[3], HEX) + "-" + String(rfid.serNum[4], HEX);
        lcd.setCursor(0, 1);
        lcd.print("NEW CARD FOUND!");
        lcd.setCursor(0, 2);
        lcd.print("UID : ");
        lcd.print(RFID);
        delay(3000);
        lcd.clear();

        String host = hostAdd;
        host += "/";
        host += RFID;
        host += "/";
        host += key;
        
        HTTPClient http;

        Serial.print("Request Link : ");
        Serial.println(host);

        http.begin(host);

        int httpCode = http.GET();
        String payload = http.getString();

        Serial.print("Response Code : ");
        Serial.println(httpCode);

        Serial.print("Returned data from server : ");
        Serial.println(payload);

        if (httpCode == 200) {
          DynamicJsonDocument doc(1024);

          //parse JSON OBJECT

          auto error = deserializeJson(doc, payload);
          if (error) {
            //  Serial.print(F("deserializeJson() failed with code!"));
            // Serial.println(error.c_str());
            return;
          }

          // Decode JSON/Extract values
          String responStatus = doc["status"].as<String>();
          String responKet = doc["ket"].as<String>();

          lcd.clear();
          lcd.setCursor(5, 1);
          lcd.print("CARD ADDED");
          lcd.setCursor(5, 2);
          lcd.print(responKet);

          Serial.println();
          Serial.print("status : ");
          Serial.println(responStatus);

          Serial.print("ket : ");
          Serial.println(responKet);
          Serial.println("-------------------");
          Serial.println();

          delay(1000);
          lcd.setCursor(0, 3);
          lcd.print("                    ");
          delay(3000);
          lcd.clear();
        }



      }
    }
    rfid.halt();
    delay(1000);
  }



}
