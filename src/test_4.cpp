#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstdint>

#include <complex>

#include "lib/fixedpoint.h"
#include "lib/taylormath.h"

int main() {

    if (std::abs(fixed32::from_float(0.02f) - fixed32(0.02f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_float(0.2f) - fixed32(0.2f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_float(2.0f) - fixed32(2.0f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_float(20.f) - fixed32(20.0f)) > 1e-4f) return 1;

    if (std::abs(fixed32::from_float(-0.02f) - fixed32(-0.02f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_float(-0.2f) - fixed32(-0.2f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_float(-2.0f) - fixed32(-2.0f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_float(-20.f) - fixed32(-20.0f)) > 1e-4f) return 1;

    if (std::abs(fixed32::from_double(0.02f) - fixed32(0.02f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_double(0.2f) - fixed32(0.2f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_double(2.0f) - fixed32(2.0f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_double(20.f) - fixed32(20.0f)) > 1e-4f) return 1;

    if (std::abs(fixed32::from_double(-0.02f) - fixed32(-0.02f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_double(-0.2f) - fixed32(-0.2f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_double(-2.0f) - fixed32(-2.0f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_double(-20.f) - fixed32(-20.0f)) > 1e-4f) return 1;

    return 0; // this is a test polygon

}
