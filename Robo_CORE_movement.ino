/***************************************************
  ROBO-CORE (movement)
  -This code is for movement part
  Made by: Md. Rasiul Alam Ratul
 ****************************************************/

#include <ESP8266WiFi.h>
#include<Servo.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "SSID"  // Wifi SSID
#define WLAN_PASS       "PASS" // wifi password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "user" // user name
#define AIO_KEY         "key" // key

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe Mask = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Mask");
Adafruit_MQTT_Subscribe Left = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Left");
Adafruit_MQTT_Subscribe Right = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Right");
Adafruit_MQTT_Subscribe Neck = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Nn");
Adafruit_MQTT_Subscribe Move = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Move");

int Mask_state=0, LeftV=0, RightV=0, NeckV=0, MoveV=0;
int pos;

/*************************** Sketch Code ************************************/

void MQTT_connect();

Servo mervo;
Servo Left_Hand;
Servo Right_Hand;
Servo NeckS;
Servo RM;
Servo LM;
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  delay(10);
  mervo.attach(D3);
  Left_Hand.attach(D4);
  Right_Hand.attach(D5);
  NeckS.attach(D6);
  RM.attach(D7);
  LM.attach(D8);
  
  // setting defult position
  mervo.write(70);
  Left_Hand.write(90);
  Right_Hand.write(90);
  delay(10);
  NeckS.write(90);
  RM.write(90);
  LM.write(90);

  Left_Hand.detach();
  
  Serial.begin(115200);
  delay(10);

  Serial.println(F("CORE"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, HIGH);
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&Mask);
  mqtt.subscribe(&Left);
  mqtt.subscribe(&Right);
  mqtt.subscribe(&Neck);
  mqtt.subscribe(&Move);
}

uint32_t x=0;

void loop() {
  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &Mask) {
      Serial.print(F("Mask State: "));
      Serial.println((char *)Mask.lastread);
      Mask_state = atoi((char *)Mask.lastread);
      if(Mask_state==1){
        mervo.write(175);
        Serial.println("Open");
        delay(5000);
        mervo.write(70);
        Serial.println("Close");
      }
    }
    else if (subscription == &Left) {
      Serial.print(F("Left: "));
      Serial.println((char *)Left.lastread);
      LeftV = atoi((char *)Left.lastread);
      Left_Hand.attach(D4);
      Left_Hand.write(LeftV);
      delay(1000);
      Left_Hand.detach();
    }
    else if (subscription == &Right) {
      Serial.print(F("Right: "));
      Serial.println((char *)Right.lastread);
      RightV = atoi((char *)Right.lastread);
      Right_Hand.write(RightV);
      delay(1000);
    }
    else if (subscription == &Neck) {
      Serial.print(F("Neck: "));
      Serial.println((char *)Neck.lastread);
      NeckV = atoi((char *)Neck.lastread);
      NeckS.write(NeckV);
      delay(1000);
    }
    else if (subscription == &Move) {
      Serial.print(F("Move: "));
      Serial.println((char *)Move.lastread);
      MoveV = atoi((char *)Move.lastread);
      
      if(MoveV==0){ //Stop
        RM.write(90);
        delay(10);
        LM.write(90);
      }
      else if(MoveV==1){ //Forward
        RM.write(5);
        for (pos = 90; pos <= 180; pos += 1) { 
          LM.write(pos);             
          delay(20);                       
        }
      }
      else if(MoveV==2){ //Backward
        LM.write(5);
        for (pos = 90; pos <= 180; pos += 1) { 
          RM.write(pos);             
          delay(20);                       
        }
      }
      else if(MoveV==3){ //Left
        RM.write(5);
        LM.write(5);
        delay(200);
      }
      else if(MoveV==4){ //Right
        for (pos = 90; pos <= 180; pos += 1) { 
          RM.write(pos); 
          LM.write(pos);            
          delay(20);                       
        }
        delay(100);
        RM.write(90);
        delay(10);
        LM.write(90);
    }
      }
      delay(1200);
      RM.write(90);
      delay(10);
      LM.write(90);
    }
  }
  /*if(! mqtt.ping()) {
    mqtt.disconnect();
  }*/
  //servo.writeMicroseconds(1000);


void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");
  digitalWrite(LED_BUILTIN, HIGH);

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 3 seconds...");
       mqtt.disconnect();
       delay(3000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
  digitalWrite(LED_BUILTIN, LOW);
}
