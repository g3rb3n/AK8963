AK8963 C++ driver.

# Example #
```
#include <iostream>
#include <AK8963.h>
#include <Point3D.h>
#include <Point3D_IO.h>

AK8963 ak;
Point3D<float> mag;
ak.magneto(mag);
cout << mag;
```
