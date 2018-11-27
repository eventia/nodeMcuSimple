#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include "PMS7003s.h"

#define TRIGGER_PIN   0
#define  DEBUG        1
#define  MEAN_NUMBER  10
#define  MAX_PM       0
#define  MIN_PM       32767
#define  VER          20181029

#ifndef    MAX_FRAME_LEN
#define    MAX_FRAME_LEN   64
#endif

#undef max
#define max(a,b) ((a)>(b)?(a):(b))
#undef min

#define min(a,b) ((a)>(b)?(b):(a))

int status = WL_IDLE_STATUS;
int pm1_0=0, pm2_5=0, pm10_0=0;
unsigned int tmp_max_pm1_0, tmp_max_pm2_5, tmp_max_pm10_0; 
unsigned int tmp_min_pm1_0, tmp_min_pm2_5, tmp_min_pm10_0; 
byte i=0;
unsigned long previousMillis = 0;
const long interval = 1000;
bool ledState = LOW;
bool startNumber =  true;

String apiKey = "AFBPM14NVQZKW281";     //  Enter your Write API key from ThingSpeak
const char *ssid =  "2G";     // replace with your wifi ssid and wpa2 key
const char *pass =  "4d10041004";
const char* server = "api.thingspeak.com";

// Thingspeak
unsigned long myChannelNumber = 161020;
const char * myWriteAPIKey = "AFBPM14NVQZKW281";

WiFiClient  client;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     
  pinMode(TRIGGER_PIN, INPUT);
  Serial.begin(9600);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
     }
  Serial.println("");
  Serial.println("WiFi connected");  
}

void thingSpeakClient(int pm1_0, int pm2_5, int pm10_0) {
	
	 if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
        {
        String postStr = apiKey;
        postStr +="&field1=";
        postStr += String(pm1_0);
        postStr +="&field2=";
        postStr += String(pm2_5);
        postStr +="&field3=";
        postStr += String(pm10_0);
        postStr +="&field4=";
        postStr += String(VER);
        postStr += "\r\n\r\n";
 
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);
 
        Serial.print("field1: ");
        Serial.print(pm1_0);
        Serial.print("field2: ");
        Serial.print(pm2_5);
        Serial.print("field3: ");
        Serial.println(pm10_0);
     }
}


void loop() {
  digitalWrite(LED_BUILTIN, LOW);    delay(500);                     
  digitalWrite(LED_BUILTIN, HIGH);    delay(500); 


  if(i==0) { 
    tmp_max_pm10_0 = tmp_max_pm2_5 = tmp_max_pm1_0  = MAX_PM;
    tmp_min_pm10_0 = tmp_min_pm2_5 = tmp_min_pm1_0  = MIN_PM;
  }
  
  if (pms7003_read()) {
    tmp_max_pm1_0  = max(PMS7003S.concPM1_0_CF1, tmp_max_pm1_0);
    tmp_max_pm2_5  = max(PMS7003S.concPM2_5_CF1, tmp_max_pm2_5);
    tmp_max_pm10_0 = max(PMS7003S.concPM10_0_CF1, tmp_max_pm10_0);
    tmp_min_pm1_0  = min(PMS7003S.concPM1_0_CF1, tmp_min_pm1_0);
    tmp_min_pm2_5  = min(PMS7003S.concPM2_5_CF1, tmp_min_pm2_5);
    tmp_min_pm10_0 = min(PMS7003S.concPM10_0_CF1, tmp_min_pm10_0);
    pm1_0 += PMS7003S.concPM1_0_CF1;
    pm2_5 += PMS7003S.concPM2_5_CF1;
    pm10_0 += PMS7003S.concPM10_0_CF1;
    i++;
  }

  if(i==MEAN_NUMBER) {

    pm1_0 = ((pm1_0-tmp_max_pm1_0-tmp_min_pm1_0)/(MEAN_NUMBER-2));
    pm2_5 = ((pm2_5-tmp_max_pm2_5-tmp_min_pm2_5)/(MEAN_NUMBER-2));
    pm10_0= ((pm10_0-tmp_max_pm10_0-tmp_min_pm10_0)/(MEAN_NUMBER-2));

    thingSpeakClient(pm1_0, pm2_5, pm10_0);
    
    delay(20000); // ThingSpeak will only accept updates every 15 seconds. 
    
    pm1_0=pm2_5=pm10_0=i=0;
  }     
}





