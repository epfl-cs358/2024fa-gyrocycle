#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>
#include "one_euro.h"

#define GEAR_RATIO 0.5
#define INITIAL_KALMAN_ANGLE -0.1
// 0.06981 corresponds to 4 deg of uncertainty on the first step
#define INITIAL_KALMAN_UNCERTAINTY 0.06981
#define TORQUE_FOR_CONSTANT_SPEED 0.018

// This defines how much the angle is influenced by the accelerometer and the gyroscope
#define ANGLE_ALPHA 0.1f
#define NUMBER_OF_MEASURE_FOR_MPU_CALIBRATION 500
#define MOVING_AVERAGE_SIZE 100

#define MINIMUM_VOLTAGE 11.3f // Volts

// Angle and uncertainty at each step
float angle = INITIAL_KALMAN_ANGLE;
float uncertainty = INITIAL_KALMAN_UNCERTAINTY;
unsigned long lastAngleUpdateTime = 0;
unsigned long currentTime = 0;

unsigned long lastLoopTime = 0;
unsigned long currentLoopTime = 0;

// model characteristics
const float mass = 2;               // in kg
const float centerOfGravity = 0.15; // in meters

// basic balancing constants
const float positionWeight = 1.2;
const float rotationWeight = 0.1;

// Which controller to use to determine what torque the flywheel should have
String controllerMode = "PID";

// Which filter to use for angle calculation
String filter = "KALMAN";

// Error terms for the PID controller
float previous_error = 0.0;
float total_error = 0.0;

// used to change the stabilization point
float angleCorrection = -0.01;

// margin of error for the angle to be considered vertical
float angleMargin = 0.00;

// PID constants
float Kp = 1.5;
float Ki = 0.0000001;
float Kd = 0.1;

// Indicates whether the current running mode is configuration or balancing
int isInConfigurationMode = 1;

// Indicates whether to print values for plotting when balancing or not
bool plotterEnabled = true;

OneEuroFilter oneEuroGyroFilter = OneEuroFilter(DEFAULT_FREQUENCY, 1.0, 0.01, DEFAULT_DCUTOFF);
OneEuroFilter oneEuroAngleAccFilter = OneEuroFilter(DEFAULT_FREQUENCY, 1.0, 0.01, DEFAULT_DCUTOFF);

float movingAverage[MOVING_AVERAGE_SIZE];
int movingAverageIndex = 0;

void switchMode()
{
  isInConfigurationMode = !isInConfigurationMode;

  // Once the state is updated, do some transition logic
  if (isInConfigurationMode)
  {
    Serial.println("Exiting balancing mode, stopping motor...");
    stopFlywheelMotor();
    Serial.println("Motor stopped.");
    Serial.println("Entering configuration mode. Type 'help' for a list of available commands.");
  }
  else
  {
    Serial.println("Preparing for balancing mode...");
    Serial.println("Resetting state variables...");
    angle = INITIAL_KALMAN_ANGLE;
    uncertainty = INITIAL_KALMAN_UNCERTAINTY;
    lastAngleUpdateTime = 0;
    currentTime = 0;
    lastLoopTime = 0;
    currentLoopTime = 0;
    total_error = 0.0;
    Serial.println("State variables reset.");
    Serial.println("Entering balancing mode.");
  }
}

void calibrateInitialAngle(void)
{
  float sum = 0;
  for (int i = 0; i < NUMBER_OF_MEASURE_FOR_MPU_CALIBRATION; i++)
  {
    float accelY, accelZ, gyroX;
    mpuMeasure(&accelY, &accelZ, &gyroX);
    sum += atan(accelY / accelZ);
  }
  angle = sum / NUMBER_OF_MEASURE_FOR_MPU_CALIBRATION;
}

void setup(void)
{

  // Start the Serial communication
  Serial.begin(500000);

  // Wait for the Serial to be opened
  while (!Serial)
    delay(10);

  delay(2000);

  // Start I2C communication with the MPU6050 gyroscope sensor
  initMpuCommunication();

  Wire.setClock(400000);
  unsigned long clock = Wire.getClock();
  Serial.print("clock:");
  Serial.println(clock);

  // Start UART communication with ODrive motor controller
  initOdriveCommunication();

  Serial.println("Calibrating MPU6050...");
  calibrateInitialAngle();
  Serial.println("MPU6050 calibrated.");

  Serial.println("Setting up RemoteXY...");
  initMotion();
  Serial.println("RemoteXY ready");

  Serial.println("Setup complete.");
  Serial.println("Starting in 3...");
  delay(1000);
  Serial.println("2...");
  delay(1000);
  Serial.println("1...");
  delay(1000);
}

void loop()
{
  if(getVbusVoltage() < MINIMUM_VOLTAGE) {
    Serial.println("MINIMUM VOLTAGE ERROR");
    safetyStop();
    return;
  }

  handleRemoteControlEvents();
  updateGyroAngle();

  // Run one iteration at a time instead of blocking in configuration or balancing mode.
  // This allows RemoteXY to handle bluetooth inputs under the hood, between two loops.
  if (isInConfigurationMode)
  {
    // In configuration mode, the user can enter commands to calibrate and monitor the system.
    configurationMode();
  }
  else
  {
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
void configurationMode()
{
  if (Serial.available() > 0)
  {
    String command = Serial.readStringUntil('\n');
    if (command.startsWith("algo "))
    {
      String parts = command.substring(5);
      if (parts == "PID" || parts == "OG")
      {
        controllerMode = parts;
        Serial.print("Controller mode set to: ");
        Serial.println(controllerMode);
      }
      else
      {
        Serial.print("Unknown controller mode: '");
        Serial.print(parts);
        Serial.println("'.");
      }
    }
    else if (command == "calibrate_mpu")
    {
      calibrateMpu();
    }
    else if (command == "clear_errors")
    {
      clearFlywheelErrors();
    }
    else if (command == "config")
    {
      Serial.println("Usage: config <Kp> <Ki> <Kd> <max_fw_speed> <max_fw_torque> <odrive_max_speed>");
    }
    else if (command.startsWith("config "))
    {
      // Extract the values from the command
      String parts = command.substring(7);
      float newKp, newKi, newKd, max_fw_speed, max_fw_torque, odrive_max_speed;
      sscanf(parts.c_str(), "%f %f %f %f %f %f", &newKp, &newKi, &newKd, &max_fw_speed, &max_fw_torque, &odrive_max_speed);

      // Update the values
      Kp = newKp;
      Ki = newKi;
      Kd = newKd;
      setFlywheelMaxSpeed(max_fw_speed);
      setFlywheelMaxTorque(max_fw_torque);
      setOdriveConfigMaxSpeed(odrive_max_speed);

      // Notify the user of the update
      Serial.print("New configuration: Kp=");
      Serial.print(Kp);
      Serial.print(", Ki=");
      Serial.print(Ki);
      Serial.print(", Kd=");
      Serial.print(Kd);
      Serial.print(", max_fw_speed=");
      Serial.print(max_fw_speed);
      Serial.print(", max_fw_torque=");
      Serial.print(max_fw_torque);
      Serial.print(", odrive_max_speed=");
      Serial.println(odrive_max_speed);
    }
    else if (command == "help")
    {
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
      Serial.println("---------------- config <Kp> <Ki> <Kd> <max_fw_speed> <max_fw_torque> <odrive_max_speed>");
      Serial.println("Sets the PID constants and the maximum speed and torque for the flywheel motor, all in one command.");
      Serial.println();
      Serial.println("---------------- debug");
      Serial.println("Prints reached maxima in prior testing and resets detectors");
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
      Serial.println("---------------- set [Kp/Ki/Kd/max_fw_speed/max_fw_torque/odrive_max_speed] <value>");
      Serial.println("Sets the named value (Kp, Ki, Kd, max_fw_speed, max_fw_torque or odrive_max_speed) to the provided value.");
      Serial.println();
      Serial.println("---------------- start");
      Serial.println("Exits the configuration mode and starts the balancing loop.");
      Serial.println("---------------- angle_margin");
      Serial.println("Sets the margin of error for the angle to be considered vertical.");
      Serial.println();
      Serial.println("===========================================");
    }
    else if (command.startsWith("plotter"))
    {
      if (command == "plotter on" || command == "plotter true" || command == "plotter 1")
      {
        plotterEnabled = true;
        Serial.println("Plotting logs are now enabled.");
      }
      else if (command == "plotter off" || command == "plotter false" || command == "plotter 0")
      {
        plotterEnabled = false;
        Serial.println("Plotting logs are now disabled.");
      }
      else
      {
        Serial.println("Usage: plotter <on/off>");
      }
    }
    else if (command == "print")
    {
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
      Serial.println("General:");
      Serial.print("Plotting enabled: ");
      Serial.println(plotterEnabled);
      Serial.println("===========================================");
    }
    else if (command == "odrive_repl")
    {
      odriveRepl();
    }
    else if (command.startsWith("set "))
    {
      String name = command.substring(4);
      float value = name.substring(name.indexOf(' ') + 1).toFloat();
      if (name.startsWith("Kp "))
      {
        Kp = value;
        Serial.print("New Kp: ");
        Serial.println(Kp);
      }
      else if (name.startsWith("Ki "))
      {
        Ki = value;
        Serial.print("New Ki: ");
        Serial.println(Ki);
      }
      else if (name.startsWith("Kd "))
      {
        Kd = value;
        Serial.print("New Kd: ");
        Serial.println(Kd);
      }
      else if (name.startsWith("max_fw_speed "))
      {
        setFlywheelMaxSpeed(value);
        Serial.print("New max flywheel speed: ");
        Serial.println(value);
      }
      else if (name.startsWith("max_fw_torque "))
      {
        setFlywheelMaxTorque(value);
        Serial.print("New max flywheel torque: ");
        Serial.println(value);
      }
      else if (name.startsWith("odrive_max_speed "))
      {
        setOdriveConfigMaxSpeed(value);
        Serial.print("New ODrive max speed: ");
        Serial.println(value);
      }
      else
      {
        Serial.print("Unknown constant: '");
        Serial.print(name);
        Serial.println("'.");
      }
    }
    else if (command == "debug")
    {
      printMaxReached();
    }
    else if (command == "start")
    {
      switchMode();
    }
    else if (command.startsWith("setpoint "))
    {
      angleCorrection = command.substring(command.indexOf(' ') + 1).toFloat();
    }
    else if (command.startsWith("angle_margin "))
    {
      angleMargin = command.substring(command.indexOf(' ') + 1).toFloat();
    }
    else
    {
      Serial.print("Unknown command: '");
      Serial.print(command);
      Serial.println("'.");
    }
  }
}

void updateGyroAngle()
{
  float accelY, accelZ, gyroX, gyroY, gyroZ;
  mpuMeasure(nullptr, &accelY, &accelZ, &gyroX, &gyroY, &gyroZ);

  if (filter == "KALMAN")
  {
    angleCalculatorKalman(accelY, accelZ, gyroX);
  }
  else if (filter == "EURO")
  {
    angleCalculatorEuro(accelY, accelZ, gyroX, gyroY, gyroZ);
  }
}

void balancingMode()
{
  // Check that the angle is within the safety bounds, otherwise stop balancing
  if (angle <= (-M_PI / 4) || angle >= (M_PI / 4))
  {
    Serial.println("Safety angle limit reached, stopping motor...");
    switchMode();
    return;
  }

  // Check that the user did not stop the balancing mode manually
  if (Serial.available() > 0)
  {
    Serial.readStringUntil('\n');
    switchMode();
    return;
  }

  currentLoopTime = micros();

  // flywheel motor input (constrained for safety)
  float input = 0;
  if (controllerMode == "PID")
  {
    float speed = getFlywheelMotorSpeed();
    float setpoint = 0 - (speed / getOdriveConfigMaxSpeed() * 0.04);
    input = pidBalancingImplementation(currentLoopTime - lastLoopTime, angle, setpoint);
  }

  // monitoring purposes
  if (plotterEnabled)
  {
    Serial.print("elapsedTime(microseconds):");
    Serial.print(currentLoopTime - lastLoopTime);
    Serial.print(",");
    Serial.print("angle:");
    Serial.print(angle, 5);
    Serial.print(",");
    Serial.print("input:");
    Serial.println(input, 5);
  }

  // float speed = getFlywheelMotorSpeed();
  // Serial.print(",");
  // Serial.print("speed:");
  // Serial.println(speed);

  // if (flywheelMotorSpeedOverUpperBound() && input > TORQUE_FOR_CONSTANT_SPEED) {
  //   setFlywheelMotorTorque(TORQUE_FOR_CONSTANT_SPEED);
  // }
  // else if (flywheelMotorSpeedUnderLowerBound() && input < -TORQUE_FOR_CONSTANT_SPEED) {
  //   setFlywheelMotorTorque(-TORQUE_FOR_CONSTANT_SPEED);
  // }
  // else {
  //   // The safety bounds are applied within the function
  //   setFlywheelMotorTorque(input);
  // }
  setFlywheelMotorTorque(input);

  lastLoopTime = currentLoopTime;
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
float ogBalancingImplementation(float gyroX, float angle)
{
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
 * @param setpoint The setpoint we want to go to
 *
 * @return float The torque that should be applied to the flywheel motor.
 */
float pidBalancingImplementation(unsigned long elapsedTime, float angle, float setpoint)
{
  // The error of this controller is the angle from the vertical
  float error = angle - angleCorrection - setpoint;
  if (-angleMargin < error && error < angleMargin)
  {
    error = 0;
    return 0;
  }
  Serial.println(error);
  // if (angle < 0) error = -error;

  // Accumulate the error for the integral (I) term
  // TODO : Should we clamp the value of total_error?
  total_error += error * elapsedTime / 1000000.0;

  // Calculate the derivative (D) term
  float derivative = (error - previous_error) / (elapsedTime / 1000000.0);

  // Update the previous error
  previous_error = error;

  unsigned long time = millis();

  // Calculate the input to the flywheel motor
  return Kp * error + Ki * total_error + Kd * derivative;
}

// calculates tilt angle from sensor readings
void angleCalculatorKalman(float accelY, float accelZ, float gyroX)
{
  unsigned long currentTime = micros();

  float acc_angle = atan(accelY / accelZ);                                            // angle calculated with accelerometer readings
  float gyro_angle = angle + (currentTime - lastAngleUpdateTime) / 1000000.0 * gyroX; // angle integrated from gyroscope readings

  float uncertainty = uncertainty + ((currentTime - lastAngleUpdateTime) / 1000000.0) * ((currentTime - lastAngleUpdateTime) / 1000000.0) * 0.00487; // 0,00487 is the assumed variance on the rotation rate (std deviation of 4 deg/s)
  // calculate kalman gain
  float gain = uncertainty / (uncertainty + 0.00274); // 0.00274 is the assumed variance of the measured angle with the accelerometer (std deviation of 3 deg)
  // angle calculated with the kalman filter
  angle = gyro_angle + gain * (acc_angle - gyro_angle);
  // update uncertainty
  uncertainty = (1 - gain) * uncertainty;

  lastAngleUpdateTime = currentTime;
}

void angleCalculatorEuro(float accelY, float accelZ, float gyroX, float gyroY, float gyroZ)
{
  unsigned long currentTime = micros();
  float filteredGyroX = oneEuroGyroFilter.filter(gyroX, (float)(currentTime / 1000000.0f));
  float predictedAngle = angle + (float)(currentTime - lastAngleUpdateTime) / 1000000.0f * filteredGyroX;
  float filteredAccAngle = oneEuroAngleAccFilter.filter((float)atan(accelY / accelZ), (float)(currentTime / 1000000.0f));

  // This correct term is the angle calculated with the accelerometer
  float newAngle = 0.0;

  // If the gyroscope is not moving, the accelerometer is more reliable
  if (filteredGyroX < 0.01f && filteredGyroX > -0.01f && gyroY < 0.1f && gyroY > -0.1f && gyroZ < 0.1f && gyroZ > -0.1f)
  {
    newAngle = (float)((1 - ANGLE_ALPHA) * predictedAngle + ANGLE_ALPHA * filteredAccAngle);
  }
  else
  {
    newAngle = (float)(ANGLE_ALPHA * predictedAngle + (1 - ANGLE_ALPHA) * filteredAccAngle);
  }
  angle = newAngle;
  lastAngleUpdateTime = currentTime;
}

void stopAll() {
  stopFlywheelMotor();
  stopPropulsionMotor();
}

void safetyStop() {
  Serial.println("SAFETY STOP TRIGGERED");
  while(1) {
    stopFlywheelMotor();
    stopPropulsionMotor();
  }
}