#include <WiFi.h>
#include <PubSubClient.h>
#include <IRremote.h>
#include <HTTPClient.h>

// not important config (not used anymore anywhere)
const char* nazwa = "iotHOME_esp";
const char* wersja = "0.6.9";

// wifi config
const char* ssid = "ssid";
const char* password = "password";

int input_pin = 23; // wpisujemy nazwę pinu, po którym nastepuje komunikacja
IRrecv irrecv(input_pin);
decode_results signals;

// mqtt config
const char* mqtt_server = "192.168.1.11";
const int mqtt_port = 1883;

long lastMsg = 0;
char msg[50];
int value = 0;

String topic[5];
String topic_status[5];
int pin[5];
boolean state[5];
int howManyPins = 3;

int sensor = 0;
int lastSensor = 0;
boolean alarm_occured = false;
const char* outlet1_state;

bool czy_przycisk = false;

WiFiClient espClient;
PubSubClient client(espClient);

void mqttAssign() {
  topic[0] = "home/outlets/outlet1";
  pin[0] = 18;
  state[0] = LOW;
  topic[1] = "home/outlets/outlet2";
  pin[1] = 19;
  state[1] = LOW;
  topic[2] = "home/outlets/outlet3";
  pin[2] = 21;
  state[2] = LOW;
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
        if (alarm_occured) {
          client.publish("home/alarms/again", "1");
        } else {
          digitalWrite(pin[i], HIGH);
          state[i] = HIGH;
        }
      }
      else if ((char)payload[0] == '0') {
        if (!alarm_occured) {
          digitalWrite(pin[i], LOW);
          state[i] = LOW;
        }
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
  pinMode(A0, INPUT);

  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  irrecv.enableIRIn(); // włączenie odbierania danych

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (irrecv.decode(&signals)) {
    irrecv.blink13(true); // mrugamy diodami w arduino

    if (signals.value == 0xFFA25D)
    {
      //      Serial.println("CH-");
      client.publish("home/remotes/main", "CH-");
    }
    if (signals.value == 0xFF629D)
    {
      //      Serial.println("CH");
      client.publish("home/remotes/main", "CH");
    }
    if (signals.value == 0xFFE21D)
    {
      //      Serial.println("CH+");
      client.publish("home/remotes/main", "CH+");
    }
    if (signals.value == 0xFF22DD)
    {
      //      Serial.println("prev");
      client.publish("home/remotes/main", "prev");
    }
    if (signals.value == 0xFF02FD)
    {
      //      Serial.println("next");
      client.publish("home/remotes/main", "next");
    }
    if (signals.value == 0xFFC23D)
    {
      //      Serial.println("playpause");
      client.publish("home/remotes/main", "playpause");
    }
    if (signals.value == 0xFFE01F)
    {
      //      Serial.println("-");
      client.publish("home/remotes/main", "-");
    }
    if (signals.value == 0xFFA857)
    {
      //      Serial.println("+");
      client.publish("home/remotes/main", "+");
    }
    if (signals.value == 0xFF906F)
    {
      //      Serial.println("EQ");
      client.publish("home/remotes/main", "EQ");
    }
    if (signals.value == 0xFF6897)
    {
      //      Serial.println("0");
      client.publish("home/remotes/main", "0");
    }
    if (signals.value == 0xFF9867)
    {
      //      Serial.println("100+");
      client.publish("home/remotes/main", "100+");
    }
    if (signals.value == 0xFFB04F)
    {
      //      Serial.println("200+");
      client.publish("home/remotes/main", "200+");
    }
    if (signals.value == 0xFF30CF)
    {
      //      Serial.println("1");
      client.publish("home/remotes/main", "1");
    }
    if (signals.value == 0xFF18E7)
    {
      //      Serial.println("2");
      client.publish("home/remotes/main", "2");
    }
    if (signals.value == 0xFF7A85)
    {
      //      Serial.println("3");
      client.publish("home/remotes/main", "3");
    }
    if (signals.value == 0xFF10EF)
    {
      //      Serial.println("4");
      client.publish("home/remotes/main", "4");
    }
    if (signals.value == 0xFF38C7)
    {
      //      Serial.println("5");
      client.publish("home/remotes/main", "5");
    }
    if (signals.value == 0xFF5AA5)
    {
      //      Serial.println("6");
      client.publish("home/remotes/main", "6");
    }
    if (signals.value == 0xFF42BD)
    {
      //      Serial.println("7");
      client.publish("home/remotes/main", "7");
    }
    if (signals.value == 0xFF4AB5)
    {
      //      Serial.println("8");
      client.publish("home/remotes/main", "8");
    }
    if (signals.value == 0xFF52AD)
    {
      //      Serial.println("9");
      client.publish("home/remotes/main", "9");
    }

    irrecv.resume(); // nasłuchujemy na następne nadanie
  }
}
