#include "lib/fixedpoint.h"

int main() {

    fixed32_s x = 15;
    fixed32_s y = 0;

    if ((x / y) != std::numeric_limits<fixed32_s>::max())
        return 1;

    x = -15;

    if ((x / y) != std::numeric_limits<fixed32_s>::lowest())
        return 2;

    return 0;
}