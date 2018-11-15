#include <ESP8266WiFi.h>
#define LED_1 2
#define ANALOG_IN A0

String apiKey = "VGYJ5PNYW12345";     //  Enter your Write API key from ThingSpeak
const char *ssid =  "mygateway";     // replace with your wifi ssid and wpa2 key
const char *pass =  "mypassword";
const char* server = "api.thingspeak.com";
int inputVal = 0;

WiFiClient client;

void setup() {
  pinMode(LED_1, OUTPUT);
  Serial.begin(115200);
  delay(20);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");  
}

void loop() {
  inputVal = analogRead(ANALOG_IN); 
  Serial.println(inputVal);
  
  digitalWrite(LED_1, LOW);
  delay(1000);
  digitalWrite(LED_1, HIGH);
  delay(1000);
      

  if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
     {  
        String postStr = apiKey;
        postStr +="&field1=";
        postStr += String(inputVal);
        postStr +="&field2=";
        postStr += String(-inputVal/2);
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
 
        Serial.print("Temperature: ");
        Serial.print(inputVal);
        Serial.print(" degrees Celcius, Humidity: ");
        Serial.print(inputVal/2);
        Serial.println("%. Send to Thingspeak.");
     }
  client.stop();
 
  Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(15000);
}
