#include <cmath>
#include "lib/fixedpoint.h"
#include "lib/polyapprox.h"

int main() {

    PolyApprox<fixed32> dynamic_object = PolyApprox<fixed32>::create<double>(sin, 10, -1, 1);

    PolyApprox<fixed32, 10> static_object = PolyApprox<fixed32, 10>::create<double>(sin, -1, 1);

    return std::floor(dynamic_object(0) + static_object(0));
}
