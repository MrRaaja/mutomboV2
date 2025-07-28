#include <Arduino.h>

extern const bool useMQTT = true; // Set to false to disable MQTT functionality


extern const char* WIFI_SSID = "";
extern const char* WIFI_PASSWORD = "";
extern const char* MQTT_SERVER = "";
extern const int MQTT_PORT = 1883;
extern const String NAME = "Stepper";
extern const char* MQTT_TOPIC = (NAME + "/commands").c_str();
extern const char* MQTT_STATUS_TOPIC = (NAME + "/status").c_str();



extern const int STEP_PIN = D7; // STEP
extern const int DIR_PIN = D8;  // DIR
extern const int EN_PIN = -1;   // ENABLE (optional)
extern int LIMIT_SWITCH_PIN = D6;

enum Command
{
  OPEN,
  CLOSE,
  HALF,
  GET_POSITION,
  RESET,
  UNKNOWN
};

extern int HOME_DIRECTION = 1;

extern int MOTOR_DELAY = 800; // Speed (less is faster, more is slower)


// === Rail Parameters ===
extern const int TOTAL_STEPS = 3130; // Full 50cm travel (10mm pulley)
extern const int STEPS_PER_LOOP = 1;
