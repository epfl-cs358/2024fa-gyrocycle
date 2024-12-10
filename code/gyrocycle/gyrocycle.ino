#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>

#define GEAR_RATIO 0.5
#define INITIAL_KALMAN_ANGLE -0.1
// 0.06981 corresponds to 4 deg of uncertainty on the first step
#define INITIAL_KALMAN_UNCERTAINTY 0.06981
#define TORQUE_FOR_CONSTANT_SPEED 0.018

// Angle and uncertainty at each step
float kalman_pred[] = {INITIAL_KALMAN_ANGLE, INITIAL_KALMAN_UNCERTAINTY};

unsigned long lastTime = 0;
unsigned long currentTime = 0;

// model characteristics
const float mass = 2; // in kg
const float centerOfGravity = 0.15; // in meters

// basic balancing constants
const float positionWeight = 1.2;
const float rotationWeight = 0.1;

// Which controller to use to determine what torque the flywheel should have
String controllerMode = "PID";

// Error terms for the PID controller
float previous_error = 0.0;
float total_error = 0.0;

// PID constants
float Kp = centerOfGravity * mass * 9.81 * GEAR_RATIO * positionWeight;
float Ki = 0.0;
float Kd = 0.0;

// Indicates whether the current running mode is configuration or balancing
int isInConfigurationMode = 1;

void switchMode() {
  isInConfigurationMode = !isInConfigurationMode;
  
  // Once the state is updated, do some transition logic
  if (isInConfigurationMode) {
    Serial.println("Exiting balancing mode, stopping motor...");
    stopFlywheelMotor();
    Serial.println("Motor stopped.");
    Serial.println("Entering configuration mode. Type 'help' for a list of available commands.");
  }
  else {
    Serial.println("Preparing for balancing mode...");
    Serial.println("Resetting state variables...");
    kalman_pred[0] = INITIAL_KALMAN_ANGLE;
    kalman_pred[1] = INITIAL_KALMAN_UNCERTAINTY;
    lastTime = 0;
    currentTime = 0;
    total_error = 0.0;
    Serial.println("State variables reset.");
    Serial.println("Entering balancing mode.");
  }
}

void setup(void) {
  // Start the Serial communication
  Serial.begin(9600);

  // Wait for the Serial to be opened
  while (!Serial)
    delay(10); 

  // Start I2C communication with the MPU6050 gyroscope sensor
  initMpuCommunication();
  
  // Start UART communication with ODrive motor controller
  initOdriveCommunication();

  Serial.println("Setup complete.");
  Serial.println("Starting in 3...");
  delay(1000);
  Serial.println("2...");
  delay(1000);
  Serial.println("1...");
  delay(1000);
}

void loop() {
  // Run one iteration at a time instead of blocking in configuration or balancing mode.
  // This allows RemoteXY to handle bluetooth inputs under the hood, between two loops.
  if (isInConfigurationMode) {
    // In configuration mode, the user can enter commands to calibrate and monitor the system.
    configurationMode();
  }
  else {
    // In balancing mode, the system tries to balance the bicycle.
    balancingMode();
  }
}

/**
 * Runs one iteration of the configuration mode. Because of how RemoteXY works, this function
 * has been designed not to block the execution of the loop.
 *
 * Hence, this function should be called inside a loop to keep the configuration mode running.
 *
 * Some commands (such as the ODrive REPL) are blocking, meaning they will not return until
 * they terminate completely.
 */
void configurationMode() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command.startsWith("algo ")) {
      String parts = command.substring(5);
      if (parts == "PID" || parts == "OG") {
        controllerMode = parts;
        Serial.print("Controller mode set to: ");
        Serial.println(controllerMode);
      }
      else {
        Serial.print("Unknown controller mode: '");
        Serial.print(parts);
        Serial.println("'.");
      }
    }
    else if (command == "calibrate_mpu") {
      calibrateMpu();
    }
    else if (command == "clear_errors") {
      clearFlywheelErrors();
    }
    else if (command == "help") {
      Serial.println("===========================================");
      Serial.println();
      Serial.println("AVAILABLE COMMANDS");
      Serial.println();
      Serial.println("===========================================");
      Serial.println();
      Serial.println("---------------- algo [PID/OG]");
      Serial.println("Sets the controller mode to PID or original (OG).");
      Serial.println();
      Serial.println("---------------- calibrate_mpu");
      Serial.println("Enters calibration mode for the MPU6050 to set the offsets.");
      Serial.println();
      Serial.println("---------------- clear_errors");
      Serial.println("Clears the errors of the ODrive driver so that the motor can keep going.");
      Serial.println();
      Serial.println("---------------- help");
      Serial.println("Displays a list of the existing commands.");
      Serial.println();
      Serial.println("---------------- print");
      Serial.println("Prints the current configuration with all necessary value to write them down somewhere.");
      Serial.println();
      Serial.println("---------------- odrive_repl");
      Serial.println("Starts a REPL session with the ODrive where you can enter commands for the ODrive directly.");
      Serial.println();
      Serial.println("---------------- set [Kp/Ki/Kd/max_fw_speed/max_fw_torque] <value>");
      Serial.println("Sets the named value (Kp, Ki or Kd) to the provided value.");
      Serial.println();
      Serial.println("---------------- debug");
      Serial.println("Prints reached maxima in prior testing and resets detectors");
      Serial.println();
      Serial.println("---------------- start");
      Serial.println("Exits the configuration mode and starts the balancing loop.");
      Serial.println();
      Serial.println("===========================================");
    }
    else if (command == "print") {
      Serial.println("===========================================");
      Serial.println();
      Serial.println("CURRENT CONFIGURATION");
      Serial.println();
      Serial.println("===========================================");
      Serial.println("ODrive:");
      printOdriveConfiguration();
      Serial.println("===========================================");
      Serial.println("MPU6050:");
      printMpuConfiguration();
      Serial.println("===========================================");
      Serial.println("PID:");
      Serial.print("Kp: ");
      Serial.println(Kp);
      Serial.print("Ki: ");
      Serial.println(Ki);
      Serial.print("Kd: ");
      Serial.println(Kd);
      Serial.println("===========================================");
    }
    else if (command == "odrive_repl") {
      odriveRepl();
    }
    else if (command.startsWith("set ")) {
      String name = command.substring(4);
      float value = name.substring(name.indexOf(' ') + 1).toFloat();
      if (name.startsWith("Kp ")) {
        Kp = value;
        Serial.print("New Kp: ");
        Serial.println(Kp);
      }
      else if (name.startsWith("Ki ")) {
        Ki = value;
        Serial.print("New Ki: ");
        Serial.println(Ki);
      }
      else if (name.startsWith("Kd ")) {
        Kd = value;
        Serial.print("New Kd: ");
        Serial.println(Kd);
      }
      else if (name.startsWith("max_fw_speed ")) {
        setFlywheelMaxSpeed(value);
        Serial.print("New max flywheel speed: ");
        Serial.println(value);
      }
      else if (name.startsWith("max_fw_torque ")) {
        setFlywheelMaxTorque(value);
        Serial.print("New max flywheel torque: ");
        Serial.println(value);
      }
      else {
        Serial.print("Unknown constant: '");
        Serial.print(name);
        Serial.println("'.");
      }
    }
    else if (command == "debug") {
      printMaxReached();
    }
    else if (command == "start") {
      switchMode();
    }
    else {
      Serial.print("Unknown command: '");
      Serial.print(command);
      Serial.println("'.");
    }
  }
}

void balancingMode() {
  // Check that the angle is within the safety bounds, otherwise stop balancing
  if (kalman_pred[0] <= (-M_PI / 4) || kalman_pred[0] >= (M_PI / 4)) {
    Serial.println("Safety angle limit reached, stopping motor...");
    switchMode();
    return;
  }

  // Check that the user did not stop the balancing mode manually
  if (Serial.available() > 0) {
    Serial.readStringUntil('\n');
    switchMode();
    return;
  }
  
  currentTime = millis(); 

  float accelY, accelZ, gyroX;
  mpuMeasure(&accelY, &accelZ, &gyroX);

  // rotation rate around X axis and angle from vertical
  angleCalculator(accelY, accelZ, gyroX, kalman_pred, currentTime - lastTime);

  // flywheel motor input (constrained for safety)
  float input = 0;
  if (controllerMode == "PID"){
    input = pidBalancingImplementation(currentTime - lastTime, kalman_pred[0]);
  }
  else if (controllerMode == "OG"){
    input = ogBalancingImplementation(gyroX, kalman_pred[0]);
  }

  // monitoring purposes
  Serial.print("angle:");
  Serial.print(kalman_pred[0]);
  Serial.print(",");
  Serial.print("gyroX:");
  Serial.print(gyroX);
  Serial.print(",");
  Serial.print("input:");
  Serial.print(input);

  float speed = getFlywheelMotorSpeed();
  Serial.print(",");
  Serial.print("speed:");
  Serial.println(speed);

  if (flywheelMotorSpeedOverUpperBound() && input > TORQUE_FOR_CONSTANT_SPEED) {
    setFlywheelMotorTorque(TORQUE_FOR_CONSTANT_SPEED);
  }
  else if (flywheelMotorSpeedUnderLowerBound() && input < -TORQUE_FOR_CONSTANT_SPEED) {
    setFlywheelMotorTorque(-TORQUE_FOR_CONSTANT_SPEED);
  }
  else {
    // The safety bounds are applied within the function
    setFlywheelMotorTorque(input);
  }
  
  lastTime = currentTime;
}

/**
 * @brief Original implementation of balancing.
 *
 * Performs some computations based on the current angle of the bike, and predicts
 * the torque that should be applied to the flywheel motor.
 * 
 * @param angle The angle from the vertical, calculated with the accelerometer.
 * @return float The torque that should be applied to the flywheel motor.
 */
float ogBalancingImplementation(float gyroX, float angle) {
  // Calculate the theoretical torque that should be applied to the flywheel motor
  float theoreticalTorque = centerOfGravity * mass * 9.81 * sin(angle) * GEAR_RATIO;

  // Calculate the input to the flywheel motor
  float input = positionWeight * theoreticalTorque + rotationWeight * gyroX;

  return input;
}

/**
 * @brief Implementation of balancing algorithm using a PID controller.
 * 
 * @param elapsedTime The time elapsed since the last iteration of the PID controller.
 * @param angle The angle from the vertical, calculated with the accelerometer.
 * 
 * @return float The torque that should be applied to the flywheel motor.
 */
float pidBalancingImplementation(long elapsedTime, float angle) {
  // The error of this controller is the angle from the vertical
  float error = 1 - cos(angle);
  if (angle < 0) error = -error;

  // Accumulate the error for the integral (I) term
  // TODO : Should we clamp the value of total_error?
  total_error += error * elapsedTime;

  // Calculate the derivative (D) term
  float derivative = (error - previous_error) / elapsedTime;

  // Update the previous error
  previous_error = error;

  // Calculate the input to the flywheel motor
  return Kp * error + Ki * total_error + Kd * derivative;
}

// calculates tilt angle from sensor readings
void angleCalculator(float accelY, float accelZ, float gyroX, float* last_step, unsigned long deltaTime) {

  float acc_angle = atan(accelY/accelZ); // angle calculated with accelerometer readings
  float gyro_angle = last_step[0] + deltaTime / 1000 * gyroX; // angle integrated from gyroscope readings


  float uncertainty = last_step[1] + (deltaTime / 1000.0) * (deltaTime / 1000.0) * 0.00487; // 0,00487 is the assumed variance on the rotation rate (std deviation of 4 deg/s)
  // calculate kalman gain
  float gain = uncertainty / (uncertainty + 0.00274); // 0.00274 is the assumed variance of the measured angle with the accelerometer (std deviation of 3 deg)
  // angle calculated with the kalman filter
  last_step[0] = gyro_angle + gain * (acc_angle - gyro_angle);
  // update uncertainty
  last_step[1] = (1 - gain) * uncertainty;

}