#include <ODriveUART.h>
#include <HardwareSerial.h>

// RX pin number used on the ESP32
#define RXD2 16
// TX pin number used on the ESP32
#define TXD2 17
// Use Serial2, available on the ESP32
#define SERIAL2 2

// Baud rate to communicate with the ODrive
#define ODRIVE_BAUD 115200

// Some safety constants
float flywheelMaxSpeed = 30;
float flywheelMaxTorque = 1;

// Create a serial communication channel between the ESP32 and the ODrive
HardwareSerial odrive_serial(SERIAL2);

// Wrap the communication channel into a library that provides commands as functions
ODriveUART odrive(odrive_serial);

/**
 * Prepares everything for a smooth communication with the ODrive.
 *
 * Assumes the default Serial has been started already to print debug messages.
 *
 * This function is blocking, meaning it will only return when the ODrive is ready.
 */
void initOdriveCommunication() {
  // Start the actual UART communication channel
  odrive_serial.begin(ODRIVE_BAUD, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Created odrive_serial communication channel.");

  // Wait for the ODrive to be available and ready
  Serial.println("Waiting for ODrive...");
  while (odrive.getState() == AXIS_STATE_UNDEFINED) {
    delay(100);
  }
  Serial.println("Found ODrive.");

  // Print the DC voltage to check the communication works
  Serial.print("DC voltage: ");
  Serial.println(odrive.getParameterAsFloat("vbus_voltage"));

  // Go to closed loop control mode, where the motor actively tries to maintain
  // its current position unless we tell it to move
  Serial.println("Enabling closed loop control...");
  while (odrive.getState() != AXIS_STATE_CLOSED_LOOP_CONTROL) {
    odrive.clearErrors();
    odrive.setState(AXIS_STATE_CLOSED_LOOP_CONTROL);
    delay(10);
  }
  Serial.println("ODrive running.");
}

void printOdriveConfiguration() {
  Serial.print("Maximum speed: ");
  Serial.println(flywheelMaxSpeed);
  Serial.print("Maximum torque: ");
  Serial.println(flywheelMaxTorque);
}

/**
 * Sets the maximum speed for the ODrive. DOES NOT set the maximum speed in the ODrive's
 * configuration itself, only locally in the MCU's code.
 */
void setFlywheelMaxSpeed(float maxSpeed) {
  flywheelMaxSpeed = maxSpeed;
}

/**
 * Sets the maximum torque for the ODrive. DOES NOT set the maximum torque in the ODrive's
 * configuration itself, only locally in the MCU's code.
 */
void setFlywheelMaxTorque(float maxTorque) {
  flywheelMaxTorque = maxTorque;
}

/**
 * Clears the errors of the ODrive.
 */
void clearFlywheelErrors() {
  odrive.clearErrors();
}

/**
 * Completely stops the flywheel motor.
 */
void stopFlywheelMotor() {
  odrive.setTorque(0);
}

/**
 * Retrieves the current speed from the ODrive and returns the value.
 */
float getFlywheelMotorSpeed() {
  return odrive.getVelocity();
}

/**
 * Returns false if the current flywheel motor speed is within safe bounds. Returns
 * true otherwise.
 */
bool flywheelMotorSpeedOutOfBounds() {
  float speed = getFlywheelMotorSpeed();
  return speed >= flywheelMaxSpeed || speed <= -flywheelMaxSpeed;
}

/**
 * Sends a command to the ODrive to set its torque to the provided value.
 *
 * Security bounds are applied within the function, meaning the provided torque
 * is clamped between a minimum and maximum torque to stay safe. The user of this
 * function can pass any torque, it will get clamped by the function.
 */
void setFlywheelMotorTorque(float torque) {
  if (torque > flywheelMaxTorque) {
    odrive.setTorque(flywheelMaxTorque);
  }
  else if (torque < -flywheelMaxTorque) {
    odrive.setTorque(-flywheelMaxTorque);
  }
  else {
    odrive.setTorque(torque);
  }
}