#ifndef _AK8963_H
#define _AK8963_H

#include <Point3D.h>
#include <I2C.h>

#include "AK8963_enums.h"

#include <SPI.h>
#include <Wire.h>

#define AK8963_ADDRESS    0x0C

namespace g3rb3n{

class AK8963
{
  private:
    I2C i2c;
    uint8_t _address;
    // Factory calibration
    Point3D<float> calibration;
    // User environment bias
    Point3D<float> bias = {0, 0, 0};
    //AK8963_UNIT _units = AK8963_UNIT_MICRO_TESLA;
    AK8963_UNIT _units = AK8963_UNIT_MILLI_GAUSS;

  public:
    AK8963();
    AK8963(uint8_t address);
    //AK8963(uint8_t address, uint8_t sda, uint8_t cls);
    ~AK8963();

    uint8_t address();
    uint8_t identification();
    int connected();

    void enableSelfTest();
    void disableSelfTest();
    bool runSelfTest();
    bool runSelfTest(Point3D<int16_t>& dest);
    
    void initialize(
      AK8963_OPERATION_MODE operationMode = AK8963_OM_CONTINUOUS_100HZ,
      AK8963_SCALE_MODE scaleMode = AK8963_SM_16BITS
    );

    void setUnits(AK8963_UNIT unit);
    
    uint8_t information();

    bool available();
    bool dataOverrun();
    bool sensorOverflow();

    uint8_t magnetic(Point3D<int16_t>& destination);
    uint8_t magnetic(Point3D<float>& destination);

    AK8963_SCALE_MODE scaleMode();
    void scaleMode(AK8963_SCALE_MODE mode);
    float resolution() const;
    
    AK8963_OPERATION_MODE operationMode();
    void operationMode(AK8963_OPERATION_MODE mode);
    
    void reset();
    bool resetting();

    void disableI2C();
    void enableI2C();

    void readCalibration(Point3D<float>& destination);

  private:
    float calibrationScale(uint8_t a);
};
}
#endif
