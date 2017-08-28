#ifndef _AK8963_REGISTER_MASKS_H
#define _AK8963_REGISTER_MASKS_H

#include <BitMasks.h>

// x02 ST1
#define AK8963_DOR    BIT_1
#define AK8963_DRDY   BIT_0

// x09 ST2
#define AK8963_BITM   BIT_4
#define AK8963_HOFL   BIT_3

// x0A CNTL1
#define AK8963_BIT    BIT_4    // Not defined in table 5.1 , but is documented in 5.8 
#define AK8963_MODE   BIT_3_0
#define AK8963_MODE3  BIT_3
#define AK8963_MODE2  BIT_2
#define AK8963_MODE1  BIT_1
#define AK8963_MODE0  BIT_0
#define AK8963_SHIFT_MODE   0

// x0B CNTL2
#define AK8963_SRST   BIT_0

// x0C ASTC
#define AK8963_MASK_SELF   BIT_6

#endif
