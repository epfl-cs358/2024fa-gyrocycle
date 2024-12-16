//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// Debug logging to Serial at 115200 can be enabled
//#define REMOTEXY_DEBUGLOG

// RemoteXY will select the connection mode and include necessary libraries
#define REMOTEXY_MODE__ESP32CORE_BLE

#include <BLEDevice.h>

// Setup RemoteXY connection settings
#define REMOTEXY_BLUETOOTH_NAME "RemoteXY"

#include <RemoteXY.h>
#include <ESP32Servo.h>

// RemoteXY GUI configuration that will be sent to the connecting device
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 53 bytes
  { 255,3,0,0,0,46,0,19,0,0,0,0,31,1,200,84,1,1,3,0,
  4,177,4,16,77,48,2,26,4,3,32,77,16,176,2,26,2,89,4,26,
  14,0,2,26,31,31,79,78,0,79,70,70,0 };

// This structure defines all the variables and events of your control interface 
// 
struct {
    // input variables
  int8_t speedSlider; // from -100 to 100
  int8_t steeringSlider; // from -100 to 100
  uint8_t start_stop; // =1 if switch ON and =0 if OFF

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

// The propulsion motor driver has two input pins IN1 and IN2 that must be
// wired to PWM pins on the microcontroller.
#define PROPULSION_PIN_1 32 // IN1
#define PROPULSION_PIN_2 33 // IN2

// Pin to wire the Servo used for steering to
#define STEERING_SERVO_PIN 15

// Limit how much the Servo can steer
#define MAX_STEERING_ANGLE 60

int isStarted = 0;

unsigned long motionLastUpdate = 0;
// Update every 50ms
const unsigned long motionUpdateInterval = 50;

// Servo object for communication with the Servo motor responsible for steering
Servo steeringServo;

void initMotion() {
  RemoteXY_Init();

  // Initialize motor control pins as output
  pinMode(PROPULSION_PIN_1, OUTPUT);
  pinMode(PROPULSION_PIN_2, OUTPUT);
  pinMode(STEERING_SERVO_PIN, OUTPUT);

  // Make sure the motors are not when starting the bike
  stopPropulsionMotor();

  // Initialize (and calibrate) the Servo
  steeringServo.attach(STEERING_SERVO_PIN);
}

void handleRemoteControlEvents() {
  RemoteXY_Handler();

  unsigned long currentTime = millis();

  // Avoid updating too often
  if (currentTime - motionLastUpdate < motionUpdateInterval) {
    return;
  }

  motionLastUpdate = currentTime;

  // Map the speed slider value (-100 to 100) to PWM signal (0 to 255)
  int motorSpeed = map(abs(RemoteXY.speedSlider), 0, 100, 0, 255);
  // Direction: forward if >= 0, backward otherwise
  bool motorDirection = RemoteXY.speedSlider >= 0;

  // Map the steering slider value (-100 to 100) to servo angle (0 to 180)
  int steeringAngle = -1.0f * map(RemoteXY.steeringSlider, 100, -100, 90 - MAX_STEERING_ANGLE, 90 + MAX_STEERING_ANGLE);

  // Update the Servo angle for steering
  steeringServo.write(steeringAngle);

  // Update the speed of the propulsion motor
  if (motorSpeed == 0) {
    stopPropulsionMotor();
  }
  else {
    if (motorDirection) {
      propulsionForward(motorSpeed);
    }
    else {
      propulsionBackward(motorSpeed);
    }
  }

  if (isStarted != RemoteXY.start_stop) {
    isStarted = RemoteXY.start_stop;
    switchMode();
  }
}

/**
 * Makes the propulsion motor go forward at the provided speed.
 *
 * The speed must be between 0 and 255 (both included). Other values will be
 * ignored (motor speed won't be changed) and an error message will be printed
 * to Serial.
 */
void propulsionForward(int motorSpeed) {
  if (motorSpeed < 0 || 255 < motorSpeed) {
    Serial.print("propulsionForward called with speed out of bounds: ");
    Serial.println(motorSpeed);
    return;
  }

  analogWrite(PROPULSION_PIN_1, motorSpeed);
  analogWrite(PROPULSION_PIN_2, LOW);
}

/**
 * Makes the propulsion motor go backward at the provided speed.
 *
 * The speed must be between 0 and 255 (both included). Other values will be
 * ignored (motor speed won't be changed) and an error message will be printed
 * to Serial.
 */
void propulsionBackward(int motorSpeed) {
  if (motorSpeed < 0 || 255 < motorSpeed) {
    Serial.print("propulsionBackward called with speed out of bounds: ");
    Serial.println(motorSpeed);
    return;
  }

  analogWrite(PROPULSION_PIN_1, LOW);
  analogWrite(PROPULSION_PIN_2, motorSpeed);
}

/**
 * Stops the propulsion motor completely. Does not block until the motor has stopped.
 */
void stopPropulsionMotor() {
  analogWrite(PROPULSION_PIN_1, LOW);
  analogWrite(PROPULSION_PIN_2, LOW);
}

bool isRemoteXYConnected() {
  return RemoteXY_isConnected();
}