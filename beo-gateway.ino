#include <SPI.h>
#include <LoRa.h>

const int csPin = 15;
const int resetPin = 5;
const int irqPin = 4;

byte localAddress = 0xAA;
byte destinationAddress = 0xBB;
long lastSendTime = 0;
int interval = 2000;
int count = 0;
int i=0;

void setup() {
  Serial.begin(9600);
  Serial.println("Start BEO GATEWAY...");
  
  pinMode(LED_BUILTIN, OUTPUT);

  LoRa.setPins(csPin, resetPin, irqPin);
  
  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true) {}
  }
  delay(2000);

  Serial.println("Waiting for receive data from node BEO");
  for (i=0;i<10;i++) {
    digitalWrite(LED_BUILTIN, LOW); 
    delay(150);                      
    digitalWrite(LED_BUILTIN, HIGH);   
    delay(150);  
  }  
}

void loop() { 
   receiveMessage(LoRa.parsePacket());

}

void receiveMessage(int packetSize) {
  if (packetSize == 0) return;

  int recipient = LoRa.read();
  byte sender = LoRa.read();
  byte incomingLength = LoRa.read();

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

  Serial.print("Received data " + incoming);
  Serial.print(" from 0x" + String(sender, HEX));
  Serial.println(" to 0x" + String(recipient, HEX));

  digitalWrite(LED_BUILTIN, LOW); 
  delay(200);                      
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(200);    
}