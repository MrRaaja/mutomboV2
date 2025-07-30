#include <ESP8266WiFi.h>
#include <PubSubClient.h>
void mqttCallback(char *topic, byte *payload, unsigned int length);

void connectWiFi();
String getCallbackInfo();
void connectWiFi();

void reconnectMQTT();
void setupMQTT();
void loopMQTT();
