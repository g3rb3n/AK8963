#include <Arduino.h>

#include "../src/AK8963.h"

#include <Point3D_Print.h>
#include <MPU9250.h>

using namespace g3rb3n;

AK8963 ak;
MPU9250 mpu;

bool sample()
{
  if (!ak.available())
    return false;
    
  //Point3D<int16_t> a;
  Point3D<float> a;
  ak.magnetic(a);
  float sum = a.x + a.y + a.z;
  float squareLength = a.x*a.x + a.y*a.y + a.z*a.z;
  //a *= 1000;
  Serial.print('(');
  Serial.print(a.x, 0);
  Serial.print(' ');
  Serial.print(a.y, 0);
  Serial.print(' ');
  Serial.print(a.z, 0);
  Serial.print(')');
  Serial.print(' ');
  Serial.print(sum, 0);
  Serial.print(' ');
  Serial.print(squareLength, 0);
  Serial.print(' ');
  Serial.print(a.x + a.y, 0);
  Serial.print(' ');
  Serial.print(a.x + a.z, 0);
  Serial.print(' ');
  Serial.print(a.y + a.z, 0);
  Serial.println();
  return true;
}

void debugSample()
{
  for (int i = 0 ; i < 10 ; )
  {
    if (sample())
      ++i;
  }
}

void debug(String s)
{
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  Serial.println(s);
  debugSample();
  Serial.println("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  Serial.println();
}

void setup() {
  Serial.begin(230400);
  Serial.println();
  Serial.println("AK8963 online");

  mpu.initialize();
  mpu.enableI2CBypass();

  while(!ak.connected())
  {
    Serial.print("Waiting for AK8963 ");
    Serial.print("AK8963 responded with : ");
    Serial.print(ak.identification());
    Serial.println();
  }

  Point3D<int16_t> st;
  bool res = ak.runSelfTest(st);
  Serial.print("Self test : ");
  Serial.print(res);
  printPoint(st, 10);
  Serial.println();
  delay(100);
  
  Point3D<float> c;
  ak.readCalibration(c);
  Serial.print("Calibration : ");
  printPoint(c, 10);
  Serial.println();
  

  ak.initialize(AK8963_OM_CONTINUOUS_100HZ, AK8963_SM_16BITS);
  delay(2000);
  debug("Initial");
}

void loop()
{
  sample();
}
