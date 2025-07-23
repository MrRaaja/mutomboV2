
#include <Arduino.h>
#include <AccelStepper.h>
#include <cctype>

const int TOTAL_STEPS = 3130; // Full 50cm travel (10mm pulley)
int currentPosition = 0;

const int STEP_PIN = -1;  // -> A4988 STEP
const int DIR_PIN = -1;   // -> A4988 DIR
const int EN_PIN = -1;    // -> A4988 ENABLE (active LOW). Set to -1 if not used.
const int LIMIT_PIN = -1; // -> Limit switch (to GND, INPUT_PULLUP)

const float MAX_SPEED = 600.0f;
const float ACCELERATION = 5000.0f;

enum Command {OPEN, CLOSE, HALF, GET_POSITION, RESET, UNKNOWN};
Command currentCommand = UNKNOWN;

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

void moveTo(long pos){
  if(pos < 0) pos = 0;
  if(pos > TOTAL_STEPS) pos = TOTAL_STEPS;  
  stepper.moveTo(pos);
}

Command parseCommand(String s) {
  s.trim(); s.toLowerCase();
  if (s == "open" || s == "1") return OPEN;
  if (s == "close" || s == "2") return CLOSE;
  if (s == "half" || s == "3") return HALF;
  if (s == "pos" || s == "4") return GET_POSITION;
  if (s == "reset" || s == "5") return RESET;
  return UNKNOWN;
}


void stepNonBlocking(int stepsToMove)
{
  int direction = (stepsToMove > 0) ? 1 : -1;
  int stepsRemaining = abs(stepsToMove);

  for (int i = 0; i < stepsRemaining; i++)
  {
    stepMotor(1, direction);
    updateCurrentPosition(currentPosition + (stepsPerLoop * direction));
    yield(); // prevent WDT reset
  }
}
void stepToPosition(int targetPosition)
{
  int stepsToMove = targetPosition - currentPosition;
  stepNonBlocking(stepsToMove);
}
void updateCurrentCommand(int cmd)
{

  switch (cmd)
  {
  case 1:
    currentCommand = OPEN;
    break;

  case 2:
    currentCommand = CLOSE;
    break;

  case 3:
    currentCommand = HALF;
    break;
  case 4:
    currentCommand = GET_POSITION;
    break;
  default:
    currentCommand = UNKNOWN;
    break;
  }
}

void updateCurrentCommand()
{
  updateCurrentCommand(Serial.readStringUntil('\n').toInt());
}

void moveToSwitch(int direction)
{
  while (digitalRead(limitSwitchPin) == HIGH)
  { // need to check if != or == idk how this works
    stepNonBlocking(direction);
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(LIMIT_PIN, INPUT_PULLUP);

  if(EN_PIN >= 0){
    pinMode(EN_PIN, OUTPUT);
    digitalWrite(EN_PIN, LOW);
  }
  if (digitalRead(LIMIT_PIN) == LOW)
  {
    Serial.println("⚠️ Limit switch is already triggered. Resetting position to 0.");
    currentPosition = TOTAL_STEPS;
  } else Serial.println("✅ Limit switch is not triggered.");


  Serial.println("Stepper ready.");
  Serial.println("Available commands: open, close, half, pos, reset");
}

void loop()
{
  if (Serial.available())
  {

    // char c = Serial.read();
    // if (c == '\n' || c == '\r') {
    //   Serial.println("🛑 Stop requested!");
    //   return;
    // }
    updateCurrentCommand();

    switch (currentCommand)
    {

    case OPEN:
      stepToPosition(0);
      Serial.println("🚪 Opened to maximum position: " + String(TOTAL_STEPS) + " steps");
      break;

    case CLOSE:

      moveToSwitch(HOME_DIRECTION);
      updateCurrentPosition(TOTAL_STEPS);
      Serial.println("🏠 Homing complete. Current position reset to " + TOTAL_STEPS);
      break;

    case HALF:
      stepToPosition(TOTAL_STEPS / 2);
      Serial.println("🚪 Moved to half position: " + String(TOTAL_STEPS / 2) + " steps");
      break;

    case GET_POSITION:
      Serial.println("📍 Current Position: " + String(currentPosition) + " steps");
      break;
    case RESET:
      Serial.println("♻️ Resetting position counter to 0 (no movement)");
      updateCurrentPosition(0);
      break;
    default:
      Serial.println("❓ Unknown command. Try: open, close, half, pos, reset");
      break;
    }
  }
}

/*
  if (digitalRead(limitSwitchPin) == LOW) {
    Serial.println("🛑 Limit switch triggered. Resetting position.");
    currentPosition = 0;
    delay(1000);
  }*/
