#include <Arduino.h>

extern const bool useMQTT; // Set to false to disable MQTT functionality


extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;
extern const char* MQTT_SERVER;
extern const int MQTT_PORT;
extern const String NAME;
extern const String MQTT_TOPIC;
extern const String MQTT_STATUS_TOPIC;
extern const String CLIENT_ID;

enum Command
{
  OPEN,
  CLOSE,
  HALF,
  GET_POSITION,
  RESET,
  UNKNOWN
};

extern const int STEP_PIN; // STEP
extern const int DIR_PIN;  // DIR
extern const int EN_PIN;   // ENABLE (optional)
extern int LIMIT_SWITCH_PIN;

extern int HOME_DIRECTION;

extern int MOTOR_DELAY; // Speed (less is faster, more is slower)


// === Rail Parameters ===
extern const int TOTAL_STEPS; // Full 50cm travel (10mm pulley)
extern const int STEPS_PER_LOOP;
