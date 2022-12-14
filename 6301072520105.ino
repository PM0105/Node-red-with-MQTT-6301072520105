#include <PubSubClient.h>
#include <WiFi.h>
#define LED_PIN 2

const char* ssid ="OPPO Reno4";// your ssid
const char* password = "88888888"; // your password

const char* mqtt_server = "192.168.205.149"; // your mqtt server IP 
const char* mqtt_username = ""; // your mqtt username
const char* mqtt_password = ""; // your mqtt password
const uint16_t mqtt_port = 1883;

WiFiClient tcpClient;// create tcp client object
PubSubClient mqttClient(tcpClient); // pass tcp client to mqtt client

uint32_t lastTimestamp = millis();

void setup_wifi(const char* ssid, const char* password){
  Serial.println("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\r\nWiFi Connected");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
}

void callBack(char* topic, byte* payload, uint16_t payload_length){
  String msg;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] :");

  for (uint16_t i = 0; i < payload_length; i++ ){
    char c = (char)payload[i];
    msg += c;
  }
  Serial.println(msg);
  
  if(msg == "ON") {
    digitalWrite(LED_PIN,LOW);
    String msg("Set LED to ON");
    mqttClient.publish("Nodered",msg.c_str());
  }
  if(msg == "OFF") {
    digitalWrite(LED_PIN,HIGH);
    String msg("Set LED to OFF");
    mqttClient.publish("Nodered",msg.c_str());
  }
  if((msg != "OFF")&&(msg != "ON")){
      String msg("Please send the correct command");
      mqttClient.publish("Nodered",msg.c_str());
  }
}

void reconnect(){
  while(!mqttClient.connected()){
    Serial.println("Attemping MQTT Connection !");
    String clientID = "ESP32Client-" + String(WiFi.macAddress());
    Serial.print("ClientID : ");
    Serial.println(clientID);
    if (mqttClient.connect(clientID.c_str(),mqtt_username,mqtt_password)){
      Serial.println("MQTT Broker Connected");
      String msg = "Hello from " + clientID;
      mqttClient.publish("Nodered",msg.c_str());
      mqttClient.subscribe("ESP32");
    }
    else{
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println("try again in 1 seconds");
      delay(1000);
    }
  }
}

void setup() {
  pinMode(LED_PIN,OUTPUT);
  Serial.begin(115200);
  setup_wifi(ssid,password);
  mqttClient.setServer(mqtt_server,mqtt_port);
  mqttClient.setCallback(callBack);
}

void loop() {
  if(!mqttClient.connected()){
    reconnect();
    
  }
  mqttClient.loop();
}
