
#include <WiFi.h>
#include <WiFiClient.h> // Thay vì WiFiClientSecure.h
#include <PubSubClient.h>


/* ===== CONFIG ===== */
const char* ssid = "Hùnn";
const char* password = "11223300";
const char* mqtt_server = "172.20.10.2";
const int mqtt_port = 1883;
/* MQTT ACCOUNT (tạo trong Mosquitto HA) */
const char* mqtt_user = "hung";
const char* mqtt_pass = "1234";
const char* student_id = "10123152";
/* MQTT TOPICS */
const char* topic_cmd = "iot/lab2/10123152/cmd";
const char* topic_state = "iot/lab2/10123152/state";
/* ================== */
WiFiClient espClient;
PubSubClient client(espClient);
/* ESP32 onboard LED */
const int LED_PIN = 2;
bool ledState = false;

/* ================== WIFI ================== */
void setup_wifi() {
Serial.print("Connecting to WiFi ");
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
/* ================== MQTT CALLBACK ================== */
void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
Serial.print(topic);
Serial.print("] ");
String message;

for (unsigned int i = 0; i < length; i++) {
message += (char)payload[i];
}
Serial.println(message);
if (message == "ON") {
ledState = true;
digitalWrite(LED_PIN, HIGH);
}
else if (message == "OFF") {
ledState = false;
digitalWrite(LED_PIN, LOW);
}
/* Publish trạng thái LED */
client.publish(topic_state, ledState ? "ON" : "OFF");
}
/* ================== MQTT RECONNECT ================== */
void reconnect() {
while (!client.connected()) {
String clientId = "ESP8266-Lab2-";

clientId += student_id;
Serial.print("Connecting to MQTT... ");
if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
Serial.println("connected");
client.subscribe(topic_cmd);
Serial.print("Subscribed to ");
Serial.println(topic_cmd);

} else {
Serial.print("failed, rc=");
Serial.print(client.state());
Serial.println(" retry in 2s");
delay(2000);
}
}
}
/* ================== SETUP ================== */
void setup() {
pinMode(LED_PIN, OUTPUT);

digitalWrite(LED_PIN, LOW);
Serial.begin(115200);
setup_wifi();
client.setServer(mqtt_server, mqtt_port);
client.setCallback(callback);
client.setKeepAlive(60);
}

/* ================== LOOP ================== */
void loop() {
if (!client.connected()) {
reconnect();
}
client.loop();
}