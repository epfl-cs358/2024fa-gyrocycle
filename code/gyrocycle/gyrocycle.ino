#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;
// manually calibrate MPU6050 readings
float accelYoffset = 0.0;
float accelZoffset = 0.0;
float gyroXoffset = 0.0;

float kalman_pred[] = {0, 4}; // angle and uncertainty at each step

unsigned long lastTime = 0;
unsigned long currentTime = 0;

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens ????

  Serial.println("Adafruit MPU6050 test!");

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
}

void loop() {
  currentTime = millis();
  // MPU6050 PART
  // Get readings from the MPU6050
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // rotation rate around X axis and angle from vertical
  float gyroX = g.gyro.x - gyroXoffset;
  angleCalculator(a.acceleration.y - accelYoffset, a.acceleration.z - accelZoffset, gyroX, kalman_pred, currentTime - lastTime);
  // Testing
  Serial.println(kalman_pred[0]);
  Serial.println(kalman_pred[1]);
  // =============================================
  lastTime = currentTime;
}

// calculates tilt angle from sensor readings
void angleCalculator(float accelY, float accelZ, float gyroX, float* last_step, unsigned long deltaTime) {

  float acc_angle = atan(accelY/accelZ); // angle calculated with accelerometer readings
  float gyro_angle = last_step[0] + deltaTime / 1000 * gyroX; // angle integrated from gyroscope readings

  float uncertainty = last_step[1] + (deltaTime / 1000) * (deltaTime / 1000) * 0.00487; // 0,00487 is the assumed variance on the rotation rate (std deviation of 4 deg/s)
  // calculate kalman gain
  float gain = uncertainty / (uncertainty + 0.00274); // 0.00274 is the assumed variance of the measured angle with the accelerometer (std deviation of 3 deg)
  // angle calculated with the kalman filter
  last_step[0] = gyro_angle + gain * (acc_angle - gyro_angle);
  // update uncertainty
  last_step[1] = (1 - gain) * uncertainty;
}
