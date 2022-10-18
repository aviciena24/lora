#include <SPI.h>
#include <LoRa.h>



byte localAddress = 0xBB;
byte destinationAddress = 0xAA;
long lastSendTime = 0;
int interval = 2000;
int count = 0;
int status=0;
int i;

int trigPin = 7;//Trig – green Jumper
int echoPin = 6;    //Echo – yellow Jumper
long duration, cm, inches;


void setup() {
  Serial.begin(9600);
  Serial.println("Start BEO...");
  pinMode(LED_BUILTIN, OUTPUT);

 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
 
  
  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true) {}
  }
  delay(1000);

  Serial.println("Sending data to BEO Gateway");
  
  for (i=0;i<10;i++) {
    digitalWrite(LED_BUILTIN, LOW); 
    delay(150);                      
    digitalWrite(LED_BUILTIN, HIGH);   
    delay(150);  
  } 

}

void loop() {
  if ((millis() - lastSendTime > interval)) {
    String sensorData = String(getSensor());
    sendMessage(sensorData);

    Serial.print("Sending data sensor : " + sensorData);
    Serial.print(" from 0x" + String(localAddress, HEX));
    Serial.println(" to 0x" + String(destinationAddress, HEX));

    lastSendTime = millis();
    interval = 3000;
  }
}

void sendMessage(String outgoing) {
  digitalWrite(LED_BUILTIN, LOW); 
  delay(200);                      
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(200);  

  LoRa.beginPacket();
  LoRa.write(destinationAddress);
  LoRa.write(localAddress);
  LoRa.write(outgoing.length());
  LoRa.print(outgoing);
  LoRa.endPacket();

  digitalWrite(LED_BUILTIN, LOW); 
  delay(200);                      
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(200);     
}

int getSensor() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    pinMode(echoPin, INPUT);
    duration = pulseIn(echoPin, HIGH);
    cm = (duration/2) / 29.1;
    inches = (duration/2) / 74;
    delay(350); 
    return cm;
}
