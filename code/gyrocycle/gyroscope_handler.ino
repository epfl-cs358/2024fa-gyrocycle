#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Interface to interact with the sensor (MPU6050)
Adafruit_MPU6050 mpu;

/*
 * Calibration values for the gyroscope. Those are not defined as constants
 * to allow for manual calibration during the setup phase, without reflashing the
 * software.
 */
float accelYoffset = -0.08;
float accelZoffset = 0.14;
float gyroXoffset = 0.0;

/**
 * Prepares everything for a smooth communication with the MPU6050 sensor.
 *
 * Assumes the default Serial has been started already to print debug messages.
 *
 * This function is blocking, meaning it will only return when the MPU6050 has
 * been found and communication has been established.
 */
void initMpuCommunication() {
  // Try finding the MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    // If the MPU6050 can't be found, stop the execution and wait for a reset
    while (1) {
      delay(1000);
    }
  }

  // Inform the user the sensor was found
  Serial.println("MPU6050 found.");

  // Configure the sensor
  Serial.println("Configuring the sensor...");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  delay(1000);
  Serial.println("Sensor configured.");

  // Inform the user of the current configuration
  printlnAccelerometerRange();
  printlnGyroRange();
  printlnFilterBandwidth();

}

/**
 * Performs one measurement on the MPU6050 and sets the corresponding values to
 * the provided pointers, already adjusted with their respective offset.
 *
 * NULL pointers can be passed to ignore a value.
 *
 * a* pointers are for acceleration on the three axis, g* pointers are for the
 * gyroscope values.
 */
void mpuMeasure(float* ay, float* az, float* gx) {
  // Get a measurement from the gyroscope
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Extract the data in the provided pointers
  if (ay != NULL) {
    *ay = a.acceleration.y - accelYoffset;
  }
  if (az != NULL) {
    *az = a.acceleration.z - accelZoffset;
  }
  if (gx != NULL) {
    *gx = g.gyro.x - gyroXoffset;
  }
}

/**
 * Prints a line to Serial such as "Accelerometer range set to: +-8G".
 */
void printlnAccelerometerRange() {
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
}

/**
 * Prints a line to Serial such as "Gyro range set to: +- 500 deg/s".
 */
void printlnGyroRange() {
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }
}

/**
 * Prints a line to Serial such as "Filter bandwidth set to: 21 Hz".
 */
void printlnFilterBandwidth() {
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }
}