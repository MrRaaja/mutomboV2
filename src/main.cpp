
#include <Arduino.h>
#include <AccelStepper.h>
#include <cctype>


const int STEP_PIN = D7; // STEP
const int DIR_PIN = D8;  // DIR
const int EN_PIN = -1;   // ENABLE (optional)

enum Command
{
  OPEN,
  CLOSE,
  HALF,
  GET_POSITION,
  RESET,
  UNKNOWN
};

const int HOME_DIRECTION = 1;


// === Rail Parameters ===
const int TOTAL_STEPS = 3130; // Full 50cm travel (10mm pulley)
int currentPosition = 0;

Command currentCommand = UNKNOWN;

// === Limit Switch (optional) ===
const int limitSwitchPin = D6;
// bool useLimitSwitch = true; no usage for now

const int stepsPerLoop = 1;



void updateCurrentPosition(int position)
{
  if (currentPosition < 0)
  {
    Serial.println("❗ Error: Current position cannot be negative. Resetting to 0.");
    currentPosition = 0;
  }
  else if (currentPosition > TOTAL_STEPS)
  {
    Serial.println("❗ Error: Current position exceeds total steps. Resetting to TOTAL_STEPS.");
    currentPosition = TOTAL_STEPS;
  }
  else
  {
    currentPosition = position;
    Serial.println("✅ Current position updated to: " + String(currentPosition) + " steps");
  }
}

//int delay = 800; //speed (less is faster, more is slower)
void stepMotor(int steps, int direction) {
  digitalWrite(DIR_PIN, direction > 0 ? HIGH : LOW);
  for (int i = 0; i < abs(steps); i++) {
    digitalWrite(STEP_PIN, 50);
    delayMicroseconds(800);  // adjust speed
    digitalWrite(STEP_PIN, 0);
    delayMicroseconds(800);
  }
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
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW); // Enable the driver
  pinMode(limitSwitchPin, INPUT_PULLUP);


  if (digitalRead(limitSwitchPin) == LOW)
  {
    Serial.println("⚠️ Limit switch is already triggered. Resetting position to 0.");
    currentPosition = TOTAL_STEPS;
  }
  else
  {
    Serial.println("✅ Limit switch is not triggered.");
  }

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
