// iotHOME
// esp32
// ESP32 Dev Module
#include <WiFi.h>
#include <PubSubClient.h>
#include <IRremote.h>
#include <HTTPClient.h>

// wifi config
const char* ssid = "multimedia_dominikanska8";
const char* password = "dominikanskawynajem";

const char* nazwa = "iotHOME_esp32";

int input_pin = 23;
IRrecv irrecv(input_pin);
decode_results signals;

// mqtt config
const char* mqtt_server = "192.168.0.11";
const int mqtt_port = 1883;

long lastMsg = 0;
char msg[50];
int value = 0;

String topic[5];
String topic_status[5];
int pin[5];
boolean state[5];
boolean lastState = 0;
int howManyPins = 3;

int sensor = 0;
int lastSensor = 0;
boolean alarm_occured = false;
const char* outlet1_state;

bool czy_przycisk = false;

unsigned long long lastMillis = 0;
int delayTime = 5000;

WiFiClient espClient;
PubSubClient client(espClient);

void mqttAssign() {
  topic[0] = "home/outlets/outlet1"; // wzmacniacz
  pin[0] = 18;
  state[0] = LOW;
  topic[1] = "home/outlets/outlet2"; // lampki
  pin[1] = 19;
  state[1] = LOW;
  topic[2] = "home/outlets/outlet3"; // przedłużacz pod biurkiem
  pin[2] = 21;
  state[2] = LOW;
  topic[3] = "home/wzmacniacz/state"; // feedback wzmacniacza
  pin[3] = 27;
  state[3] = LOW;
  topic[4] = "home/outlets/outlet4"; // testowy przekaźnik
  pin[4] = 13;
  state[4] = LOW;
}

void mqttSubscribe() {
  for (int i = 0; i < howManyPins; i++) {
    char mqtt_topic[40];
    topic[i].toCharArray(mqtt_topic, 40);
    client.subscribe(mqtt_topic);
  }
}

void mqttAction(char* topic_received, char* payload) {
  for (int i = 0; i < howManyPins; i++) {
    char mqtt_topic[40];
    topic[i].toCharArray(mqtt_topic, 40);

    if (topic[i] == String(topic_received)) {
      if ((char)payload[0] == '1') {
        digitalWrite(pin[i], HIGH);
        state[i] = HIGH;
      }
      else if ((char)payload[0] == '0') {
        digitalWrite(pin[i], LOW);
        state[i] = LOW;
      }
      else if ((char)payload[0] == 't') {
        if (state[i] == LOW) {
          digitalWrite(pin[i], HIGH);
          state[i] = HIGH;
        } else {
          digitalWrite(pin[i], LOW);
          state[i] = LOW;
        }
      }
      char* state_buf = "0";
      if (state[i]) state_buf = "1";
      
      if (String(topic_received) == "home/outlets/outlet1") {
        client.publish("home/outlets/outlet1/state", state_buf);
      }
      if (String(topic_received) == "home/outlets/outlet2") {
        client.publish("home/outlets/outlet2/state", state_buf);
      }
      if (String(topic_received) == "home/outlets/outlet3") {
        client.publish("home/outlets/outlet3/state", state_buf);
      }
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  char payload_all[30];
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    payload_all[i] = + payload[i];
  }
  Serial.println();
  mqttAction(topic, payload_all);
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    String clientId = nazwa;
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected!");
      mqttSubscribe();
    } else {
      delay(5000);
    }
  }
}

void setup() {
  mqttAssign();
  pinMode(pin[0], OUTPUT);
  pinMode(pin[1], OUTPUT);
  pinMode(pin[2], OUTPUT);
  pinMode(pin[3], INPUT_PULLUP);
  pinMode(pin[4], OUTPUT);
  pinMode(A0, INPUT);

  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  irrecv.enableIRIn();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  lastMillis = millis();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (irrecv.decode(&signals)) {
    if (signals.value == 0xFFA25D) client.publish("home/remotes/main", "CH-");
    if (signals.value == 0xFF629D) client.publish("home/remotes/main", "CH");
    if (signals.value == 0xFFE21D) client.publish("home/remotes/main", "CH+");
    if (signals.value == 0xFF22DD) client.publish("home/remotes/main", "prev");
    if (signals.value == 0xFF02FD) client.publish("home/remotes/main", "next");
    if (signals.value == 0xFFC23D) client.publish("home/remotes/main", "playpause");
    if (signals.value == 0xFFE01F) client.publish("home/remotes/main", "-");
    if (signals.value == 0xFFA857) client.publish("home/remotes/main", "+");
    if (signals.value == 0xFF906F) client.publish("home/remotes/main", "EQ");
    if (signals.value == 0xFF6897) client.publish("home/remotes/main", "0");
    if (signals.value == 0xFF9867) client.publish("home/remotes/main", "100+");
    if (signals.value == 0xFFB04F) client.publish("home/remotes/main", "200+");
    if (signals.value == 0xFF30CF) client.publish("home/remotes/main", "1");
    if (signals.value == 0xFF18E7) client.publish("home/remotes/main", "2");
    if (signals.value == 0xFF7A85) client.publish("home/remotes/main", "3");
    if (signals.value == 0xFF10EF) client.publish("home/remotes/main", "4");
    if (signals.value == 0xFF38C7) client.publish("home/remotes/main", "5");
    if (signals.value == 0xFF5AA5) client.publish("home/remotes/main", "6");
    if (signals.value == 0xFF42BD) client.publish("home/remotes/main", "7");
    if (signals.value == 0xFF4AB5) client.publish("home/remotes/main", "8");
    if (signals.value == 0xFF52AD) client.publish("home/remotes/main", "9");
    irrecv.resume();
  }
}
