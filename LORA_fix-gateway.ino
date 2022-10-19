#include <SPI.h>
#include <LoRa.h>
#include <ESP8266Webhook.h>
#include <ESP8266WiFi.h>

#define _SSID     "TKJ SMKN 1 ADIWERNA"      // Your WiFi SSID
#define _PASSWORD "keselbardadikuli"  // Your WiFi Password
#define KEY       "fyvIAzYFgRujcDca8fNYnm4QfhJ8cWpDjIhK4NhGjUB"        // Webhooks Key
#define EVENT     "banjir_iot"      // Webhooks Event Name
#define CECKPIN   16 

const int csPin = 15;
const int resetPin = 5;
const int irqPin = 4;
int c=0;
int err=0;
int recipient;
byte sender, incomingLength;
String dataSensor, node;

byte localAddress = 0xAA;
byte destinationAddress = 0xBB;
long lastSendTime = 0;
int interval = 2000;
int count = 0;
int i=0;

Webhook webhook(KEY, EVENT);    // Create an object.

void setup() {
  Serial.begin(9600);
  pinMode(CECKPIN, OUTPUT);
  Serial.println("Start BEO GATEWAY...");


//========================Setup Wifi==========================//

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(CECKPIN, LOW);
    delay(250);
    digitalWrite(CECKPIN, HIGH);
    delay(250);
    Serial.print("-");
  }

  Serial.println("");
  Serial.println("WiFi Connected");

  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  digitalWrite(LED_BUILTIN, HIGH);

//========================end Setup Wifi==========================//


//========================Setup LORA==========================//
  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa init failed. Check your connections.");
    digitalWrite(CECKPIN, LOW);
    while (true) {}
  }

  delay(2000);

  Serial.println("Waiting for receive data from node BEO");
  for (i=0;i<10;i++) {
    digitalWrite(CECKPIN, LOW); 
    delay(150);                      
    digitalWrite(CECKPIN, HIGH);   
    delay(150);  
  }  
}

void loop() { 
  receiveMessage(LoRa.parsePacket());


  if(atol(dataSensor) < 35 ) {
    webhook.trigger(String(dataSensor), random(2000), String(node));
    int response = webhook.trigger();

    if(response == 200) {
      Serial.println("OK");
      delay(30000);
    }
    else
      Serial.println("Failed");
  }
  c++;
  if (c<50) {
  pushData(dataSensor,random(2000));
  c=0;
  }
}


void pushData(String Value1, String Value2) {
  String url="http://iot.smkn1adw.sch.id/add.php?sn=saqmx202&aqi=";
  String aqiPUSH = String(Value1);
  String penghubung = "&suhu=";
  String suhuPUSH = String(Value2);
  String joined = url+ suhuPUSH + penghubung + aqiPUSH; 
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status 
    HTTPClient http;  
    
    http.begin(joined);  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
 
    if (httpCode > 0) { //Check the returning code
 
    String payload = http.getString();
    Serial.println(payload);

    if (payload.indexOf("ok")>0){
      Serial.println("Berhasil Sinkron");
      //delay(1000);
      err=0;
      }else{
        Serial.println("Gagal Sinkron");
        err++;
        }
    }
 
    http.end();   //Close connection
  }

}

void receiveMessage(int packetSize) {
  if (packetSize == 0) return;

  recipient = LoRa.read();
  sender = LoRa.read();
  incomingLength = LoRa.read();

  delay(5000);

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {
    Serial.println("Error: Data tidak lengkap / tidak sesuai format");
    return;
  }

  if (recipient != localAddress) {
    Serial.println("Error: Recipient address does not match local address");
    return;
  }
  digitalWrite(LED_BUILTIN, LOW); 
  delay(200);                      
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(200); 

  dataSensor = incoming;
  node = String(sender, HEX);
  Serial.print("Received data " + incoming);
  Serial.print(" from 0x" + String(sender, HEX));
  Serial.println(" to 0x" + String(recipient, HEX));

  digitalWrite(LED_BUILTIN, LOW); 
  delay(200);                      
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(200);
}
