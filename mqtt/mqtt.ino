#include <WiFi.h>
#include <PubSubClient.h>

// --- Ρυθμίσεις Δικτύου ---
const char* ssid = "Vodafone_5G-00680";
const char* password = "FjKdEggetXNdETk5";
const char* mqtt_server = "192.168.2.51"; // Η IP του Raspberry Pi

WiFiClient espClient;
PubSubClient client(espClient);

int count = 0; // Ο αριθμός που θα στέλνουμε

void setup_wifi() {
  delay(10);
  Serial.begin(115200);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Επανάληψη μέχρι να συνδεθούμε στον Broker
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Δημιουργία μοναδικού ID για τον Client
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Αύξηση του αριθμού
  count++;

  // Μετατροπή του αριθμού σε String για το MQTT
  char msg[10];
  sprintf(msg, "%d", count);

  Serial.print("Publishing message: ");
  Serial.println(msg);

  // Αποστολή στο Topic
  client.publish("test/counter", msg);

  delay(2000); // Αναμονή 2 δευτερόλεπτα
}