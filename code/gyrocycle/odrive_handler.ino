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
float flywheelMaxSpeed = 80;
float flywheelMaxTorque = 0.5;
// boolean to check if the limits have been hit
float absSpeedReached = 0;
float absTorqueReached = 0;

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

  // Wait for the ODrive to be available and ready, allow for calibration at startup if needed
  Serial.println("Waiting for ODrive...");
  int odriveState = odrive.getState();
  while (odriveState != AXIS_STATE_IDLE && odriveState != AXIS_STATE_CLOSED_LOOP_CONTROL) {
    delay(100);
    odriveState = odrive.getState();
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

/**
 * Starts a REPL session between the user and the ODrive. The user can enter any command they see fit,
 * and those commands will be sent to the ODrive. One special command can be used to end the REPL session.
 */
void odriveRepl() {
  String sendMessage;
  String receivedMessage;

  bool keepGoing = true;

  Serial.println("===========================================");
  Serial.println();
  Serial.println("Starting ODrive REPL session");
  Serial.println();
  Serial.println("Type 'done', 'exit' or 'quit' to end the");
  Serial.println("session.");
  Serial.println();
  Serial.println("===========================================");

  while (keepGoing) {
    while (odrive_serial.available() > 0) {
      char receivedChar = odrive_serial.read();
      if (receivedChar == '\n') {
        // Print the received message to the Serial monitor
        Serial.println(receivedMessage);
        // Reset the received message
        receivedMessage = "";
      } else {
        // Append characters to the received message
        receivedMessage += receivedChar;
      }
    }

    if (Serial.available() > 0) {
      char inputChar = Serial.read();
      if (inputChar == '\n') {
        // Send the message through Serial1 with a newline character
        Serial.println("> " + sendMessage);
        if (sendMessage == "done" || sendMessage == "exit" || sendMessage == "quit") {
          keepGoing = false;
        }
        else {
          odrive_serial.println(sendMessage);
        }
        // Reset the message
        sendMessage = "";
      } else {
        // Append characters to the message
        sendMessage += inputChar;
      }
    }
  }

  Serial.println("===========================================");
  Serial.println();
  Serial.println("End of the REPL session");
  Serial.println();
  Serial.println("===========================================");
}

void printOdriveConfiguration() {
  Serial.print("Maximum torque: ");
  Serial.println(flywheelMaxTorque);
  Serial.print("ODrive configuration maximum speed: ");
  Serial.println(getOdriveConfigMaxSpeed());
}

/**
 * Retrieves the maximum speed from the ODrive's configuration (not from the software configuration).
 */
float getOdriveConfigMaxSpeed() {
  return odrive.getParameterAsFloat("axis0.controller.config.vel_limit");
}

/**
 * Sets the maximum speed on the ODrive's configuration (not in the software configuration).
 */
void setOdriveConfigMaxSpeed(float maxSpeed) {
  odrive.setParameter("axis0.controller.config.vel_limit", String(maxSpeed));
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
  float speed = odrive.getVelocity();

  // used for deugging
  if (speed > absSpeedReached) {
    absSpeedReached = speed;
  }
  if (-speed > absSpeedReached) {
    absSpeedReached = -speed;
  }

  return speed;
}

/**
 * Sends a command to the ODrive to set its torque to the provided value.
 *
 * Security bounds are applied within the function, meaning the provided torque
 * is clamped between a minimum and maximum torque to stay safe. The user of this
 * function can pass any torque, it will get clamped by the function.
 */
void setFlywheelMotorTorque(float torque) {
  // used for deugging
  if (torque > absTorqueReached) {
    absTorqueReached = torque;
  }
  if (-torque > absTorqueReached) {
    absTorqueReached = -torque;
  }


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
/**
 * Debug function
 */
void printMaxReached(){
  Serial.print("Velocity reached during testing: ");
  Serial.println(absSpeedReached);
  Serial.print("Torque reached during testing (desired torque before clamping): ");
  Serial.println(absTorqueReached);
  Serial.println("Resetting detectors...");
  absSpeedReached = 0;
  absTorqueReached = 0;
}

float getVbusVoltage() {
  return odrive.getParameterAsFloat("vbus_voltage");
}