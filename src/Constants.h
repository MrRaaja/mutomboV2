#include <Arduino.h>

extern const int STEP_PIN = D7; // STEP
extern const int DIR_PIN = D8;  // DIR
extern const int EN_PIN = -1;   // ENABLE (optional)
extern int limitSwitchPin = D6;

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

extern int motorDelay = 800; // Speed (less is faster, more is slower)


// === Rail Parameters ===
extern const int TOTAL_STEPS = 3130; // Full 50cm travel (10mm pulley)
extern const int stepsPerLoop = 1;
