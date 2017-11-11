#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid     = "SSID";
const char* password = "PASSWD";

const char* host = "test.com";

const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "B4 F1 34 53 8F D9 84 B1 F0 08 10 64 92 EF E9 8D 1C 2E 45 E1";

int greenLed = 5; //D1
int yellowLed = 4; //D2
int redLed = 0; //D3 //pwm
int blueLed = 14; //D5

String total_activity = "0";
String total_connected = "0";
  

void setup() {
  Serial.begin(115200);
  delay(10);


  // We start by init led
  pinMode(greenLed, OUTPUT);
  digitalWrite(greenLed, 0);
  
  pinMode(yellowLed, OUTPUT);
  digitalWrite(yellowLed, 0);

   pinMode(redLed, OUTPUT);
  digitalWrite(redLed, 0);

  pinMode(blueLed, OUTPUT);
  digitalWrite(blueLed, 0);
  
  //connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop() {
  
  delay(50000);

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
   WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("Certificate matches");
  } else {
    Serial.println("Certificate doesn't match");
  }

  // We now create a URI for the request
  String url = "/api/api-test";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") +url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(100);

  
  Serial.println("Respond:");
  
    String json = "";
  boolean httpBody = false;
  // Read all the lines of the reply from server and get json info
  while (client.available()) {
    String line = client.readStringUntil('\r');
    if (!httpBody && line.charAt(1) == '{') {
      httpBody = true;
    }
    if (httpBody) {
      json += line;
    }
  }
  StaticJsonBuffer<400> jsonBuffer;
  Serial.println("Got data:");
  Serial.println(json);
  JsonObject& root = jsonBuffer.parseObject(json);
  String data = root["datas"];
  
  String total_activity = root["datas"]["total_activity"];
  String total_connected = root["datas"]["connected"];
  Serial.println(data);
  
  if(total_activity.toInt() > 0){
    digitalWrite(greenLed, 1);
    Serial.println("greenLed > 0 ");
  }
  if(total_activity.toInt() > 1 ){
    digitalWrite(yellowLed, 1);
     Serial.println("yellowLed > 1 ");
  }
  if(total_activity.toInt() >= 3){
    digitalWrite(redLed, 1);
     Serial.println("redLed >= 3 ");
  }
   if(total_activity.toInt() < 3){
    digitalWrite(redLed, 0);
    Serial.println("redLed < 3 ");
  }
  
  if(total_activity.toInt() == 0){
      digitalWrite(greenLed, 0);
      digitalWrite(yellowLed, 0);
      digitalWrite(redLed, 0);
       Serial.println("total°activity 0");
    }
 Serial.println(total_connected.toInt() > 0);
  if(total_connected.toInt() > 0){
    digitalWrite(blueLed, 1);
     Serial.println("blue led 1");
  }else {
    digitalWrite(blueLed, 0);
     Serial.println("blue led 0");
    }
  Serial.println("closing connection");
  delay(10);
}
