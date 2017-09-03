
#include "AK8963.h"
#include "AK8963_registers.h"
#include "AK8963_register_masks.h"
#include "AK8963_register_values.h"

#include <Point3D_Math.h>
#include <Point3D_Print.h>

namespace g3rb3n
{

AK8963::AK8963():
  i2c(AK8963_ADDRESS)
{
}

AK8963::AK8963(uint8_t address):
  i2c(address)
{
}
/*
AK8963::AK8963(uint8_t sda, uint8_t scl, uint8_t address):
  _address(address)
{
  Wire.begin(sda, scl);
}
*/
AK8963::~AK8963()
{}

uint8_t AK8963::address()
{
  return _address;
}

uint8_t AK8963::identification()
{
  return i2c.readByte(AK8963_WHO_AM_I);
}

int AK8963::connected()
{
  return identification() == AK8963_IDENTIFIER;
}

uint8_t AK8963::information()
{
  return i2c.readByte(AK8963_INFO);
}

bool AK8963::runSelfTest()
{
  Point3D<int16_t> data;
  return runSelfTest(data);
}

bool AK8963::runSelfTest(Point3D<int16_t>& data)
{
  operationMode(AK8963_OM_POWER_DOWN);
  scaleMode(AK8963_SM_16BITS);
  enableSelfTest();
  operationMode(AK8963_OM_SELF_TEST);
  while(!available()){}
  magnetic(data);
  disableSelfTest();
  operationMode(AK8963_OM_POWER_DOWN);
  return 
  -200  <= data.x && data.x <= 200 &&
  -200  <= data.y && data.y <= 200 &&
  -3200 <= data.z && data.z <= -800;
}

bool AK8963::available()
{
  return i2c.readMaskBit(AK8963_ST1, AK8963_DRDY);
}

bool AK8963::dataOverrun()
{
  return i2c.readMaskBit(AK8963_ST1, AK8963_DOR);
}

uint8_t AK8963::magnetic(Point3D<float>& destination)
{
  Point3D<int16_t> r;
  magnetic(r);
  destination = static_cast<Point3D<float>>(r);
//  printPoint(destination, 10);
  destination *= calibration;
//  printPoint(destination, 10);
  destination *= resolution();
//  printPoint(destination, 10);
  destination -= bias;
//  printPoint(destination, 10);
}

uint8_t AK8963::magnetic(Point3D<int16_t>& destination)
{
  uint8_t rawData[7];  // x/y/z gyro register data, ST2 register stored here, must read ST2 at end of data acquisition
  i2c.readBytes(AK8963_HXL, 7, &rawData[0]);  // Read the six raw data and ST2 registers sequentially into data array
  uint8_t c = rawData[6]; // End data read by reading ST2 register
  if(c & 0x08) {
    Serial.println("ERR Overflow");
    return 2;
  }
  destination.x = ((int16_t)rawData[1] << 8) | rawData[0] ;  // Turn the MSB and LSB into a signed 16-bit value
  destination.y = ((int16_t)rawData[3] << 8) | rawData[2] ;  // Data stored as little Endian
  destination.z = ((int16_t)rawData[5] << 8) | rawData[4] ;
  return 0;
}

void AK8963::readCalibration(Point3D<float>& destination)
{
  operationMode(AK8963_OM_POWER_DOWN);
  delay(10);
  operationMode(AK8963_OM_FUSE_ROM_ACCESS);
  delay(10);
  
  uint8_t rawData[3];  // x/y/z gyro calibration data stored here
  i2c.readBytes(AK8963_ASAX, 3, &rawData[0]);
  Point3D<uint8_t> r(rawData[0], rawData[1], rawData[2]);
  destination.x = calibrationScale(r.x);
  destination.y = calibrationScale(r.y);
  destination.z = calibrationScale(r.z);

  Serial.println("calibration x y z ");
  Serial.print(r.x);
  Serial.print(' ');
  Serial.print(r.y);
  Serial.print(' ');
  Serial.print(r.z);
  Serial.print(' ');
  printPoint(destination, 10);
  Serial.println();
  operationMode(AK8963_OM_POWER_DOWN);
  delay(10);
}

float AK8963::calibrationScale(uint8_t a)
{
  return ((static_cast<float>(a) - 128) / 256 + 1);
}

bool AK8963::sensorOverflow()
{
  return i2c.readMaskBit(AK8963_ST2, AK8963_HOFL);
}

AK8963_SCALE_MODE AK8963::scaleMode()
{
  if (!i2c.readMaskBit(AK8963_CNTL1, AK8963_BIT))
    return AK8963_SM_14BITS;
  return AK8963_SM_16BITS;
}

void AK8963::scaleMode(AK8963_SCALE_MODE mode)
{
  if (mode)
    i2c.writeMaskSet(AK8963_CNTL1, AK8963_BIT);
  else
    i2c.writeMaskClear(AK8963_CNTL1, AK8963_BIT);
}

void AK8963::setUnits(AK8963_UNIT units)
{
  _units = units;
}

float AK8963::resolution() const
{
  float r;
  switch (scaleMode())
  {
    // Possible magnetometer scales (and their register bit settings) are:
    // 14 bit resolution (0) and 16 bit resolution (1)
    // Scale in micro Tesla / LSB
    case AK8963_SM_14BITS: r = 4912./8190.;   break;
    case AK8963_SM_16BITS: r = 4912./32760.0; break;
    default:        return 0;
  }
  switch (_units)
  {
    case AK8963_UNIT_MICRO_TESLA: return r;
    case AK8963_UNIT_TESLA: return r / 1000000;
    case AK8963_UNIT_MILLI_GAUSS: return 10. * r;
    case AK8963_UNIT_GAUSS: return r / 100.;
    default: return r;
  }
}  

AK8963_OPERATION_MODE AK8963::operationMode()
{
  return (AK8963_OPERATION_MODE)i2c.readMaskShift(AK8963_CNTL1, AK8963_MODE, AK8963_SHIFT_MODE);
}

void AK8963::operationMode(AK8963_OPERATION_MODE mode)
{
  i2c.writeMaskShiftValue(AK8963_CNTL1, AK8963_MODE, AK8963_SHIFT_MODE, mode);
}

void AK8963::reset()
{
 i2c.writeMaskSet(AK8963_CNTL2, AK8963_SRST);
}

bool AK8963::resetting()
{
  return i2c.readMaskBit(AK8963_CNTL2, AK8963_SRST);
}

void AK8963::enableSelfTest()
{
  i2c.writeMaskSet(AK8963_ASTC, AK8963_MASK_SELF);
}

void AK8963::disableSelfTest()
{
  i2c.writeMaskClear(AK8963_ASTC, AK8963_MASK_SELF);
}

/**
 * Disables I2C bus interface.
 * After this call it is impossible to write other value to I2CDIS register.
 * To enable I2C bus interface, reset AK8963 or input start condition 8 times continuously.  
 */
void AK8963::disableI2C()
{
  i2c.writeMaskSet(AK8963_I2CDIS, AK8963_I2C_DISABLE);  
}

/**
 * Once I2C bus interface is disabled, it is impossible to write other value to I2CDIS register.
 * To enable I2C bus interface, reset AK8963 or input start condition 8 times continuously.  
 */
void AK8963::enableI2C()
{
}

/*
 * Configures the magnetometer for continuous 100 Hz read and highest resolution.
 * Read the factory calibration.
 * Set scale mode to 16 bits.
 * Enable continuous 100 Hz sample rates.
 */
void AK8963::initialize(
  AK8963_OPERATION_MODE _operationMode,
  AK8963_SCALE_MODE _scaleMode
)
{
  readCalibration(calibration);
  scaleMode(_scaleMode);
  operationMode(_operationMode);
  delay(10);
}

}
