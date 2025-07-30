#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// === Update these with your values ===
const char *WIFI_SSID = "neoja";
const char *WIFI_PASSWORD = "neojaajoen";
const char *MQTT_SERVER = "192.168.1.200";
const int MQTT_PORT = 1883;
String info = "";

// === Globals ===
WiFiClient espClient;
PubSubClient client(espClient);
//PLEASE

const String CLIENT_ID = "Stepper_" + String(ESP.getChipId());
const String MQTT_TOPIC = "Stepper/command";

// === MQTT callback - just print received messages ===
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  String message = "";
  for (unsigned int i = 0; i < length; i++)
  {
    message += ((char)payload[i]);
  }
  info = message;

}
extern String getCallbackInfo(){
    return info;
}

// === Connect to WiFi ===
extern void connectWiFi()
{
  Serial.print("Connecting to WiFi ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP: " + WiFi.localIP().toString());
}

// === Connect (or reconnect) to MQTT broker ===
extern void reconnectMQTT()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection with client ID: ");
    Serial.println(CLIENT_ID);

    if (client.connect(CLIENT_ID.c_str()))
    {
      Serial.println("MQTT connected!");
      client.subscribe(MQTT_TOPIC.c_str());
    }
    else
    {
      Serial.print("Failed to connect, rc=");
      Serial.print(client.state());
      Serial.println(" — trying again in 2 seconds");
      delay(2000);
    }
  }
}

extern void setupMQTT()
{
  connectWiFi();

  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(mqttCallback);
}

extern void loopMQTT(){
    
  if (!client.connected())
  {
    reconnectMQTT();
  }
  client.loop();
}
