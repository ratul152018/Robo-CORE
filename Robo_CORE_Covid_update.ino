#include <ESP8266WiFi.h>                                                    
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal.h>


const int rs = D1, en = D2, d4 = D4, d5 = D5, d6 = D6, d7 = D7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const char* ssid = "ssid";              //WIFI SSID Name
const char* password = "pass";        //WIFI Password
const char* host = "api.thingspeak.com";  //We read the data from this host
const int httpPortRead = 80;

const char* url1 = "/apps/thinghttp/send_request?api_key=____key___________";     // api
int To_remove;      //There are some irrelevant data on the string and here's how I keep the index
//of those characters
String Cases, Deaths, Recovered, Data_Raw;
int i = 0;

WiFiClient client;      
HTTPClient http;

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("COVID-19 UPDATE"); 
  lcd.setCursor(1, 1);
  lcd.print(" BANGLADESH");  

  Serial.begin(115200);
  WiFi.disconnect();         
  delay(1000);
  WiFi.begin(ssid, password);
  Serial.println("Connected to the WiFi network"); 
  Serial.println(WiFi.localIP());
}

void loop() {


  if ( http.begin(host, httpPortRead, url1))     
  {
    int httpCode = http.GET();               
    if (httpCode > 0)
    {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {

        Data_Raw = http.getString();   
        //Serial.println(" ");
        //Serial.println(Data_Raw);
        Serial.println(" ");
       
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Deaths:");
        lcd.setCursor(2, 1);
        Serial.print("  DEATH:: ");
        for (i = 8; i <= 12; i++) {
          Serial.print(Data_Raw[i]);
          lcd.print(Data_Raw[i]);
          delay(200);
        }
        delay(3000);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Recovered:");
        lcd.setCursor(2, 1);
        Serial.print("  RECOVERED:: ");
        for (i = 13; i <= 18; i++) {
          Serial.print(Data_Raw[i]);
          lcd.print(Data_Raw[i]);
          delay(200);
        }
        delay(3000);
        
         lcd.clear();
        Serial.print("CASE:: ");
        lcd.setCursor(0, 0);
        lcd.print("Cases:");
        lcd.setCursor(2, 1);
        for (i = 0; i <= 7; i++) {
          Serial.print(Data_Raw[i]);
          lcd.print(Data_Raw[i]);
          delay(200);
        }
        delay(3000);
      }
    }
    else //If we can't get data
    {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
  else //If we can't connect to the HTTP
  {
    Serial.printf("[HTTP} Unable to connect\n");
  }

  while (WiFi.status() != WL_CONNECTED)     //In case the Wifi connetion is lost
  {

    WiFi.disconnect();
    delay(1000);

    WiFi.begin(ssid, password);
    Serial.println("Reconnecting to WiFi..");
    delay(10000);
  }
}
