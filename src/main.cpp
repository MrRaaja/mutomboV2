
#include <Arduino.h>
#include <Constants.h>
#include <MqttConection.h>

Command currentCommand;
int currentPosition;


void updateCurrentCommand(int cmd);
void updateCurrentPosition(int position);
void stepMotor(int steps, int direction);
void stepWithoutBlocking(int stepsToMove);
void stepToPosition(int targetPosition);
void moveToSwitch(int direction);
void updateCurrentCommandMQTT(String message)
{
  if (message == "open")
    updateCurrentCommand(1);
  else if (message == "close")
    updateCurrentCommand(2);
  else if (message == "half")
    updateCurrentCommand(3);
  else if (message == "pos")
    updateCurrentCommand(4);
  else if (message == "reset")
    updateCurrentCommand(5);
  else
    updateCurrentCommand(0);

  
}

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

void stepMotor(int steps, int direction)
{
  digitalWrite(DIR_PIN, direction > 0 ? HIGH : LOW);
  for (int i = 0; i < abs(steps); i++)
  {
    digitalWrite(STEP_PIN, 50);
    delayMicroseconds(MOTOR_DELAY); // adjust speed
    digitalWrite(STEP_PIN, 0);
    delayMicroseconds(MOTOR_DELAY);
  }
}
void stepWithoutBlocking(int stepsToMove)
{
  int direction = (stepsToMove > 0) ? 1 : -1;
  int stepsRemaining = abs(stepsToMove);

  for (int i = 0; i < stepsRemaining; i++)
  {
    stepMotor(1, direction);
    updateCurrentPosition(currentPosition + (STEPS_PER_LOOP * direction));
    yield(); // prevent WDT reset
  }
}
void stepToPosition(int targetPosition)
{
  int stepsToMove = targetPosition - currentPosition;
  stepWithoutBlocking(stepsToMove);
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
  while (digitalRead(LIMIT_SWITCH_PIN) == HIGH)
  { // need to check if != or == idk how this works
    stepWithoutBlocking(direction);
  }
}

void setup()
{
  currentPosition = 0;
  currentCommand = UNKNOWN;
  Serial.begin(115200);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW); // Enable the driver
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);

  if (digitalRead(LIMIT_SWITCH_PIN) == LOW)
  {
    Serial.println("⚠️ Limit switch is already triggered. Resetting position to 0.");
    currentPosition = TOTAL_STEPS;
  }
  else
  {
    Serial.println("✅ Limit switch is not triggered.");
  }
  

  Serial.println("Stepper ready.");
  setupMQTT();
  Serial.println("MQTT ready! Send commands to topic: " + NAME + "/command");
  Serial.println("Available commands: open, close, half, pos, reset");
}

void loopSerial()
{
  if (Serial.available())
  {
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

void loop()
{
  if(useMQTT){
    loopMQTT();
    updateCurrentCommandMQTT(getCallbackInfo());
  }
  else{
    loopSerial();
  }
}
