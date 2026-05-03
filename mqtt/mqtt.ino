#include <WiFi.h>
#include <PubSubClient.h>

//όνομα και κωδικος δικτύου που θα συνδεθούμε
const char* ssid = "AndroidAP_5216";
const char* password = "hz99skfhup4d7s3";
//raspberry pi ip
const char* mqtt_server = "192.168.116.118";
String ip_address = "";
//εκκινηση wifi και mqtt
WiFiClient espClient;
PubSubClient client(espClient);

int count = 0; // Ο αριθμός που θα στέλνουμε

void setup_wifi() {
  //συνδεση στο wifi
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
  ip_address = String(WiFi.localIP());
}

void reconnect() {
  //Για την συνδεση στο mqtt
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Δημιουργία μοναδικού ID για τον Client
    String clientId = "Client-";
    clientId += String(ip_address);

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
  //σύνδεση με wifi και σετάρισμα mqtt server
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  //μεχρι να υπάρξει επιτυχής σύνδεση στο mqtt
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
  client.publish("monitor/sensor1", msg);

  delay(2000); // Αναμονή 2 δευτερόλεπτα
}