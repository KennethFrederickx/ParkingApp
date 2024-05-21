#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "MSI1247";
const char* password = "4cU4{627";

// MQTT server details
const char broker[] = "test.mosquitto.org";
int port = 1883;
const char topic[] = "/traffic/light";

// Pin definitions
int R_LED = 14; // GPIO pin for Red LED
int Y_LED = 12; // GPIO pin for Yellow LED
int G_LED = 13; // GPIO pin for Green LED

WiFiClient espClient;
PubSubClient client(espClient);

// Reconnect counter and limit
int reconnect_attempts = 0;
const int max_reconnect_attempts = 5;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize LED pins
  pinMode(R_LED, OUTPUT);
  pinMode(Y_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);

  // Set all LEDs off
  digitalWrite(R_LED, LOW);
  digitalWrite(Y_LED, LOW);
  digitalWrite(G_LED, LOW);

  // Connect to WiFi
  setup_wifi();
  
  // Set up MQTT client
  client.setServer(broker, port);
  client.setCallback(callback);
}

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to WiFi network: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Convert payload to string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  // Control LEDs based on message
  if (message == "red") {
    digitalWrite(R_LED, HIGH);
    digitalWrite(Y_LED, LOW);
    digitalWrite(G_LED, LOW);
  } else if (message == "yellow") {
    digitalWrite(R_LED, LOW);
    digitalWrite(Y_LED, HIGH);
    digitalWrite(G_LED, LOW);
  } else if (message == "green") {
    digitalWrite(R_LED, LOW);
    digitalWrite(Y_LED, LOW);
    digitalWrite(G_LED, HIGH);
  } else {
    digitalWrite(R_LED, LOW);
    digitalWrite(Y_LED, LOW);
    digitalWrite(G_LED, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientID = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientID.c_str())) {
      Serial.println("connected");
      client.subscribe(topic);
      reconnect_attempts = 0; // Reset attempt counter
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" - try again in 5 seconds");

      reconnect_attempts++;
      if (reconnect_attempts >= max_reconnect_attempts) {
        Serial.println("Max reconnect attempts reached. Resetting device.");
        ESP.restart(); // Reset the device after exceeding attempts
      }

      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
