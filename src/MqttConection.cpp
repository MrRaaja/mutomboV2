
#include <Constants.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <MqttConection.h>

// === Globals ===
WiFiClient espClient;
PubSubClient client(espClient);
String mqttInfo = "";
// PLEASE

// === MQTT callback - just print received messages ===
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  String message = "";
  for (unsigned int i = 0; i < length; i++)
  {
    
    message += ((char)payload[i]);
  }
  Serial.println("MESSAGE: " + message);
  mqttInfo = message;
}
String getCallbackInfo()
{
 // Serial.println("MQTT Callback Info: " + mqttInfo);
  return mqttInfo;
}

// === Connect to WiFi ===
void connectWiFi()
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
void reconnectMQTT()
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

void setupMQTT()
{
  connectWiFi();

  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(mqttCallback);
}

void loopMQTT()
{

  if (!client.connected())
  {
    reconnectMQTT();
  }
  client.loop();
  
}
