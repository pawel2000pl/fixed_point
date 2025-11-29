#include <cstring>
#include "lib/fixedpoint.h"

#include <iostream>

int main() {

    fixed32_s x = 15;
    fixed32_s y = 0;

    if ((x / y) != std::numeric_limits<fixed32_s>::max())
        return 1;

    x = -15;

    if ((x / y) != std::numeric_limits<fixed32_s>::lowest())
        return 2;

    char buf[80];

    fixed32_s::buf_cast(-2147483648).toCharBuf(buf);
    if (strcmp(buf, "-inf"))
        return 3;

    fixed32_s(-58.25).toCharBuf(buf);
    if (strcmp(buf, "-58.25"))
        return 4;

    fixed32_s(58.25).toCharBuf(buf);
    if (strcmp(buf, "58.25"))
        return 4;
    
    if (fixed32_s::fromCharBuf("852") != 852)
        return 5;    

    if (fixed32_s::fromCharBuf("+852") != 852)
        return 6;

    if (fixed32_s::fromCharBuf("-852") != -852)
        return 7;

    if (fixed32_s::fromCharBuf("1.25") != fixed32_s::fraction(5, 4))
        return 8;

    if (fixed32_s::fromCharBuf("-1.25") != -fixed32_s::fraction(5, 4))
        return 9;

    if (fixed32_s::fromCharBuf("1.250000000000000000000000000000001") != fixed32_s::fraction(5, 4))
        return 10;

    return 0;
}