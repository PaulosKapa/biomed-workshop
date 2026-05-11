#include <WiFi.h>
#include <PubSubClient.h>

// Pins
int adcPin = 1;     // Το pin που διαβάζει τον τελεστικό
int alertLED = 2;   // Το pin του LED

//μεταβλητές κυκλώματος
float Gain = 4.7;      //κέρδος αντιστάσεων (4.7k/1k)
float Vref = 0.22;     //τάση ποτενσιόμετρου (σε Volts)

// WiFi & MQTT
const char* ssid = "wifi_name";
const char* password = "password";
const char* mqtt_server = "ip_address_of_raspberry_pi";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.begin(115200);
  //σύνδεση wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}
//σύνδεση mqtt
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32-HealthMonitor-" + String(WiFi.localIP());
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      delay(5000);
    }
  }
}

void setup() {
  //pin, mqtt, wifi setup 
  pinMode(adcPin, INPUT);
  pinMode(alertLED, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  //συνδεση mqtt
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //adc ανάγνωση
  int rawADC = analogRead(adcPin);

  // μετατροπή σε τάση εξόδου του ενισχυτή
  float Vout = (rawADC * 3.3) / 4095.0;

  //υπολογισμός θερμοκρασίας
  // Vout = Gain * (V_sensor - Vref)  =>  V_sensor = (Vout/Gain) + Vref
  float V_sensor = (Vout / Gain) + Vref;
  float temperature = V_sensor / 0.01; // 10mV ανά βαθμό

  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.println(" C");

  //αποστολή στο nodered
  char msg[10];
  dtostrf(temperature, 1, 2, msg); // Μετατροπή float σε string
  //αλλξέ το βάση που ακούει το node-red
  client.publish("monitor/sensor1", msg);

  //έλεγχος led
  if (temperature > 38.0 || temperature < 35.0) {
      digitalWrite(alertLED, HIGH);
    
  } else {
    digitalWrite(alertLED, LOW); // Κλειστό αν όλα είναι καλά
  }

  delay(1500); // Συνολική αναμονή ανά μέτρηση
}