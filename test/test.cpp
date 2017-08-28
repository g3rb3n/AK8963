#include <Arduino.h>
#include <unity.h>

#include "../src/AK8963.h"

#ifdef UNIT_TEST

using namespace g3rb3n;

AK8963 mpu;

void isConnected(void) {
  TEST_ASSERT_EQUAL(true, mpu.connected());
}

void measures(void) {
  Point3D<float> a;
  mpu.magnetic(a);
  float squareLength = a.x*a.x + a.y*a.y + a.z*a.z;
  TEST_ASSERT_FLOAT_WITHIN(0.1, 1.0, squareLength);
}

void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);
  UNITY_BEGIN();

  RUN_TEST(isConnected);
  
  mpu.initialize();
}

void loop() {
  uint8_t count = 10;
  for (uint8_t i = 0 ; i < 10 ; ++i)
  {
    while (!mpu.available()){}
    RUN_TEST(measures);
  }
  UNITY_END();
}

#endif
