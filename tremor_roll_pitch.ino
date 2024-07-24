#include <Wire.h>
#include <MPU6050.h>
#include <Servo.h>

MPU6050 mpu;
Servo servo1;
Servo servo2;

const int servo1Pin = 9;
const int servo2Pin = 10;
int16_t ax, ay, az, gx, gy, gz;
float rollAngle = 0.0, pitchAngle = 0.0, filteredRollAngle = 0.0, filteredPitchAngle = 0.0;
const float alpha = 0.9;
const float dt = 0.1; 

void setup() {
  Wire.begin();
  Serial.begin(9600);
  mpu.initialize();
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo1.write(90); 
  servo2.write(90);
}

void loop() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float accelRoll = atan2(ay, az) * 180 / PI;
  float accelPitch = atan2(-ax, az) * 180 / PI;

  float gyroRollRate = gx / 131.0;
  float gyroPitchRate = gy / 131.0;

  filteredRollAngle = 0.9 * (filteredRollAngle + gyroRollRate * 0.1) + (1 - 0.9) * accelRoll;
  filteredPitchAngle = 0.9 * (filteredPitchAngle + gyroPitchRate * 0.1) + (1 - 0.9) * accelPitch;

  int servo1Position = map(filteredRollAngle, -90, 90, 180, 0);
  int servo2Position = map(filteredPitchAngle, -90, 90, 180, 0);

  moveServoSmoothly(servo1, servo1Position);
  moveServoSmoothly(servo2, servo2Position);

  // Serial.print("Roll Angle: ");
  // Serial.print(filteredRollAngle);
  // Serial.print(" | Servo1 Position: ");
  // Serial.print(servo1Position);
  // Serial.print(" | Pitch Angle: ");
  // Serial.print(filteredPitchAngle);
  // Serial.print(" | Servo2 Position: ");
  // Serial.println(servo2Position);

  delay(10);
}

void moveServoSmoothly(Servo &servo, int targetPosition) {
  int currentServoPosition = servo.read();
  int step = 1;
  if (targetPosition > currentServoPosition) {
    for (int pos = currentServoPosition; pos <= targetPosition; pos += step) {
      servo.write(pos);
      delay(5); 
    }
  } else {
    for (int pos = currentServoPosition; pos >= targetPosition; pos -= step) {
      servo.write(pos);
      delay(5); 
    }
  }
}