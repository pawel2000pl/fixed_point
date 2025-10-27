#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstdint>

#include "lib/fixedpoint.h"
#include "lib/taylormath.h"

int main() {

    if (std::abs(fixed32::from_float(0.02f) - fixed32::from_ieee754_stable(0.02f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_float(0.2f) - fixed32::from_ieee754_stable(0.2f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_float(2.0f) - fixed32::from_ieee754_stable(2.0f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_float(20.f) - fixed32::from_ieee754_stable(20.0f)) > 1e-4f) return 1;

    if (std::abs(fixed32::from_float(-0.02f) - fixed32::from_ieee754_stable(-0.02f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_float(-0.2f) - fixed32::from_ieee754_stable(-0.2f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_float(-2.0f) - fixed32::from_ieee754_stable(-2.0f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_float(-20.f) - fixed32::from_ieee754_stable(-20.0f)) > 1e-4f) return 1;

    if (std::abs(fixed32::from_double(0.02f) - fixed32::from_ieee754_stable(0.02f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_double(0.2f) - fixed32::from_ieee754_stable(0.2f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_double(2.0f) - fixed32::from_ieee754_stable(2.0f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_double(20.f) - fixed32::from_ieee754_stable(20.0f)) > 1e-4f) return 1;

    if (std::abs(fixed32::from_double(-0.02f) - fixed32::from_ieee754_stable(-0.02f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_double(-0.2f) - fixed32::from_ieee754_stable(-0.2f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_double(-2.0f) - fixed32::from_ieee754_stable(-2.0f)) > 1e-4f) return 1;
    if (std::abs(fixed32::from_double(-20.f) - fixed32::from_ieee754_stable(-20.0f)) > 1e-4f) return 1;

    return 0;

}
