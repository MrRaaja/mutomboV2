#include <Arduino.h>
#include <Stepper.h>

// === Stepper Configuration ===
const int stepsPerRevolution = 200;
const int IN1 = D1;  // GPIO5
const int IN2 = D2;  // GPIO4
const int IN3 = D5;  // GPIO14
const int IN4 = D6;  // GPIO12
Stepper stepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

// === Rail Parameters ===
const int totalTravelSteps = 3130;  // Full 50cm travel (10mm pulley)
int currentPosition = 0;

// === Limit Switch (optional) ===
const int limitSwitchPin = D7;
bool useLimitSwitch = true;

void stepNonBlocking(int stepsToMove) {
  int direction = (stepsToMove > 0) ? -1 : 1;
  int stepsRemaining = abs(stepsToMove);

  for (int i = 0; i < stepsRemaining; i++) {
    stepper.step(direction);
    yield();  // prevent WDT reset
  }
}

void moveTo(int targetPosition) {
  int stepsToMove = targetPosition - currentPosition;
  Serial.println("🔄 Moving from " + String(currentPosition) + " to " + String(targetPosition) + " (" + String(stepsToMove) + " steps)");
  stepNonBlocking(stepsToMove);
  currentPosition = targetPosition;
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
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "home") {
      while (digitalRead(limitSwitchPin)==HIGH)  // Replace 'condition' with your actual condition
      {
        stepNonBlocking(-1);  // Move towards the limit switch
      }
      currentPosition = 0;  // Reset position after homing
      Serial.println("🏠 Homing complete. Current position reset to 0.");
      {
        /* code */
      }
      
    } 
    if (command == "open") {
      moveTo(totalTravelSteps);
    }
    else if (command == "close") {
      while (digitalRead(limitSwitchPin)==HIGH)  // Replace 'condition' with your actual condition
      {
        stepNonBlocking(-1);  // Move towards the limit switch
      }
      currentPosition = 0;  // Reset position after homing
      Serial.println("🏠 Homing complete. Current position reset to 0.");
      {
    }}
    else if (command == "half") {
      moveTo(totalTravelSteps / 2);
    }
    else if (command == "pos") {
      Serial.println("📍 Current Position: " + String(currentPosition) + " steps");
    }
    else if (command == "reset") {
      Serial.println("♻️ Resetting position counter to 0 (no movement)");
      currentPosition = 0;
    }
    else {
      Serial.println("❓ Unknown command. Try: open, close, half, pos, reset");
    }
  }
/*
  if (digitalRead(limitSwitchPin) == LOW) {
    Serial.println("🛑 Limit switch triggered. Resetting position.");
    currentPosition = 0;
    delay(1000);
  }*/
}
