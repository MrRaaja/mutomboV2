#include <Arduino.h>
#include <Constants.h>

extern const bool useMQTT = true; // Set to false to disable MQTT functionality


extern const char* WIFI_SSID = "neoja";
extern const char* WIFI_PASSWORD = "neojaajoen";
extern const char* MQTT_SERVER = "192.168.1.200";
extern const int MQTT_PORT = 1883;
extern const String NAME = "Stepper";
// extern const char* MQTT_USERNAME = "neojaasd";
// extern const char* MQTT_PASSWORD = "neojaajoasden";
extern const String MQTT_TOPIC = (NAME + "/commands");
extern const String MQTT_STATUS_TOPIC = (NAME + "/status");
extern const String CLIENT_ID = (NAME + "_ID");


extern const int IN1 = D1;  // GPIO5
extern const int IN2 = D2;  // GPIO4
extern const int IN3 = D5;  // GPIO14
extern const int IN4 = D6;  // GPIO12

extern int LIMIT_SWITCH_PIN = D7;


extern int HOME_DIRECTION = -1;

extern int MOTOR_DELAY = 800; // Speed (less is faster, more is slower)


// === Rail Parameters ===
extern const int TOTAL_STEPS = 3130; // Full 50cm travel (10mm pulley)
extern const int STEPS_PER_LOOP = 1;
