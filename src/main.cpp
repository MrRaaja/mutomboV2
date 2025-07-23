#include <Arduino.h>
#include <Stepper.h>
#include <cctype>

// === Stepper Configuration ===
const int stepsPerRevolution = 200;
const int IN1 = D1;  // GPIO5
const int IN2 = D2;  // GPIO4
const int IN3 = D5;  // GPIO14
const int IN4 = D6;  // GPIO12
Stepper stepper(stepsPerRevolution, IN1, IN3, IN2, IN4);


enum Command{
  OPEN,
  CLOSE,
  HALF,
  GET_POSITION,
  RESET,
  UNKNOWN
};

const int HOME_DIRECTION = -1;




// === Rail Parameters ===
const int TOTAL_STEPS = 3130;  // Full 50cm travel (10mm pulley)
int currentPosition = 0;

Command currentCommand = UNKNOWN;

// === Limit Switch (optional) ===
const int limitSwitchPin = D7;
//bool useLimitSwitch = true; no usage for now


const int stepsPerLoop = 1;


void updateCurrentPosition(int position) {
  if (currentPosition < 0) {
    Serial.println("❗ Error: Current position cannot be negative. Resetting to 0.");
    currentPosition = 0;
  }
  else if (currentPosition > TOTAL_STEPS) {
    Serial.println("❗ Error: Current position exceeds total steps. Resetting to TOTAL_STEPS.");
    currentPosition = TOTAL_STEPS;
  }
  else {
    currentPosition = position;
    Serial.println("✅ Current position updated to: " + String(currentPosition) + " steps");
  }
}
void stepNonBlocking(int stepsToMove) {
  int direction = (stepsToMove > 0) ? -1 : 1;
  int stepsRemaining = abs(stepsToMove);

  for (int i = 0; i < stepsRemaining; i++) {
    stepper.step(stepsPerLoop * direction);
    updateCurrentPosition(currentPosition + (stepsPerLoop * direction));
    yield();  // prevent WDT reset
  }
}
void stepToPosition(int targetPosition) {
  int stepsToMove = targetPosition - currentPosition;
  stepNonBlocking(stepsToMove);
}
void updateCurrentCommand(int cmd){

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

void updateCurrentCommand(){
  updateCurrentCommand(Serial.readStringUntil('\n').toInt());
}

void moveToSwitch(int direction) {
  while(digitalRead(limitSwitchPin)==HIGH){ // need to check if != or == idk how this works
    stepNonBlocking(direction);
  }  
}



void setup() {
  Serial.begin(115200);
  stepper.setSpeed(120);  
  pinMode(limitSwitchPin, INPUT_PULLUP);

  if (digitalRead(limitSwitchPin) == LOW) {
    Serial.println("⚠️ Limit switch is already triggered. Resetting position to 0.");
    currentPosition = 0;
  } else {
    Serial.println("✅ Limit switch is not triggered.");
  }

  Serial.println("Stepper ready.");
  Serial.println("Available commands: open, close, half, pos, reset");
}

void loop() {
  if (Serial.available()) {

    updateCurrentCommand();

    switch (currentCommand)
    {

      case OPEN:
        stepToPosition(TOTAL_STEPS);
        Serial.println("🚪 Opened to maximum position: " + String(TOTAL_STEPS) + " steps");
        break;

      case CLOSE:
      
        moveToSwitch(HOME_DIRECTION);
        updateCurrentPosition(0);
        Serial.println("🏠 Homing complete. Current position reset to 0.");
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
