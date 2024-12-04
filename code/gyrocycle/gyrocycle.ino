#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>

#define GEAR_RATIO 0.5

float kalman_pred[] = {0, 0.06981}; // angle and uncertainty at each step, 0.06981 corresponds to 4 deg of uncertainty on the first step

unsigned long lastTime = 0;
unsigned long currentTime = 0;

// model characteristics
const float mass = 1; // in kg
const float centerOfGravity = 0.15; // in meters

// basic balancing constants
const float positionWeight = 1.2;
const float rotationWeight = 0.1;



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
  // Gyroscope readings printed out, waiting for start command
  while (Serial.available() == 0 || Serial.readStringUntil('\n') != "start") {

    currentTime = millis(); 

    float accelY, accelZ, gyroX;
    mpuMeasure(&accelY, &accelZ, &gyroX);

    // print values for calibration
    Serial.print(accelY);
    Serial.print(",");
    Serial.print(accelZ);
    Serial.print(",");
    Serial.println(gyroX);

    // rotation rate around X axis and angle from vertical
    angleCalculator(accelY, accelZ, gyroX, kalman_pred, currentTime - lastTime);

    Serial.print("angle:");
    Serial.println(kalman_pred[0]);

    lastTime = currentTime;
  }
  // Balancing loop
  while (kalman_pred[0] > (-M_PI / 4) && kalman_pred[0] < (M_PI / 4) && Serial.available() == 0) {
    
    currentTime = millis(); 

    float accelY, accelZ, gyroX;
    mpuMeasure(&accelY, &accelZ, &gyroX);

    // rotation rate around X axis and angle from vertical
    angleCalculator(accelY, accelZ, gyroX, kalman_pred, currentTime - lastTime);


    // theoretical torque from gravity
    float theoreticalTorque = centerOfGravity * mass * 9.81 * sin(kalman_pred[0]) * GEAR_RATIO;

    // flywheel motor input (constrained for safety)
    float input = positionWeight * theoreticalTorque + rotationWeight * gyroX;

    // monitoring purposes
    Serial.print("angle:");
    Serial.print(kalman_pred[0]);
    Serial.print(",");
    Serial.print("theoreticalTorque:");
    Serial.print(theoreticalTorque);
    Serial.print(",");
    Serial.print("gyroX:");
    Serial.print(gyroX);
    Serial.print(",");
    Serial.print("input:");
    Serial.println(input);

    float speed = getFlywheelMotorSpeed();
    if (flywheelMotorSpeedOutOfBounds()) {
      stopFlywheelMotor();
    }
    else {
      // The safety bounds are applied within the function
      setFlywheelMotorTorque(input);
    }
  
    lastTime = currentTime;
  }
  // end of programm, stop motor and infinite wait
  stopFlywheelMotor();
  Serial.println("end");
  while (true) {
    delay(1000);
  }

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