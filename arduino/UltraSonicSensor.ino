/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-hc-sr04-ultrasonic-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/
#include <WiFi.h>
#include <ArduinoMqttClient.h>

const char* SSID = "MSI1247";
const char* WiFiPassword = "4cU4{627";

const char* broker = "test.mosquitto.org";
const int port = 1883;
const char* topic = "/sensor/ultrasonic";

WiFiClient net;
MqttClient mqttClient(net);

const int trigPin = 12;
const int echoPin = 35;

//define sound speed in cm/uS
#define SOUND_SPEED 0.0343

long duration;
float distanceCm;

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, WiFiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  ConnectToMQTT();
}


void ConnectToMQTT() {
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);
 
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }
 
  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {
  if (!mqttClient.connected()) {
    Serial.println("Lost MQTT connection. Reconnecting...");
    while (!mqttClient.connect(broker, port)) {
      delay(5000);
      Serial.println("Reconnect failed. Retrying...");
    }
    Serial.println("Reconnected.");
  }

  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);

  mqttClient.beginMessage(topic);
  mqttClient.print(distanceCm);
  mqttClient.endMessage();
  
  delay(1000);
}