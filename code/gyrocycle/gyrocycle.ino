#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>

Adafruit_MPU6050 mpu;
// manually calibrate MPU6050 readings
const float accelYoffset = -0.08;
const float accelZoffset = 0.14;
const float gyroXoffset = 0.0;

float kalman_pred[] = {0, 0.06981}; // angle and uncertainty at each step, 0.06981 corresponds to 4 deg of uncertainty on the first step

unsigned long lastTime = 0;
unsigned long currentTime = 0;

// model characteristics
const float mass = 1; // in kg
const float centerOfGravity = 0.15; // in meters

// basic balancing constants
const float positionWeight = 1;
const float rotationWeight = 0.1;
const float gear_ratio = 0.08; // should be 0.5 I think



void setup(void) {
  Serial.begin(9600);

  while (!Serial)
    delay(10); 

  Serial.println("Found Adafruit MPU6050 chip");

  // Try to initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  delay(100);
  // =============================================
  initOdriveCommunication();
  // =============================================

  Serial.println("SET OFFSETS AND MAX SPEED AND ACCELERATION CORRECTLY!");
  delay(1000);
}

void loop() {
  // Gyroscope readings printed out, waiting for start command
  while (Serial.available() == 0 || Serial.readStringUntil('\n') != "start") {

    currentTime = millis(); 

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // print values for calibration
    Serial.print(a.acceleration.y - accelYoffset);
    Serial.print(",");
    Serial.print(a.acceleration.z - accelZoffset);
    Serial.print(",");
    Serial.println(a.gyro.x - gyroXoffset);


    // rotation rate around X axis and angle from vertical
    float gyroX = g.gyro.x - gyroXoffset;
    angleCalculator(a.acceleration.y - accelYoffset, a.acceleration.z - accelZoffset, gyroX, kalman_pred, currentTime - lastTime);

    Serial.print("angle:");
    Serial.println(kalman_pred[0]);

    lastTime = currentTime;
  }
  // Balancing loop
  while (kalman_pred[0] > (-M_PI / 4) && kalman_pred[0] < (M_PI / 4) && Serial.available() == 0) {
    
    currentTime = millis(); 

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // rotation rate around X axis and angle from vertical
    float gyroX = g.gyro.x - gyroXoffset;
    angleCalculator(a.acceleration.y - accelYoffset, a.acceleration.z - accelZoffset, gyroX, kalman_pred, currentTime - lastTime);

    

    // theoretical torque from gravity
    float theoreticalTorque = centerOfGravity * mass * 9.81 * sin(kalman_pred[0]) * gear_ratio;

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