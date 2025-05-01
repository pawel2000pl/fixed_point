#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>

#include "lib/fixedpoint.h"
#include "lib/taylormath.h"


int status_code = 0;
unsigned long long test_no = 0;

using fixed64 = fixedpoint<long long int>;

int main() {


    for (float x=-10;x<10;x+=2.5e-2) {
        test_no++;
        float a = (float)taylor::cos<fixed64>(fixed64(x));
        float b = std::cos(x);
        if (std::abs(a - b) > 2e-3) {
            std::cout << "Error at test " << test_no << " (cos) at x = " << x << " : " << " expected: " << b << " got: " << a << std::endl;
            status_code = 1;
        }
    }


    for (float x=-10;x<10;x+=2.5e-2) {
        test_no++;
        float a = (float)taylor::sin<fixed64>(fixed64(x));
        float b = std::sin(x);
        if (std::abs(a - b) > 2e-3) {
            std::cout << "Error at test " << test_no << " (sin) at x = " << x << " : " << " expected: " << b << " got: " << a << std::endl;
            status_code = 1;
        }
    }


    for (float x=-1;x<1;x+=1e-2) {
        test_no++;
        float a = (float)taylor::asin<fixed64>(fixed64(x));
        float b = std::asin(x);
        if (std::abs(a - b) > 0.01) {
            std::cout << "Error at test " << test_no << " (asin) at x = " << x << " : " << " expected: " << b << " got: " << a << std::endl;
            status_code = 1;
        }
    }


    for (float x=1e-3;x<10;x*=1+1e-2) {
        test_no++;
        float a = (float)taylor::ln<fixed64>(fixed64(x));
        float b = std::log(x);
        if (std::abs(a - b) > 0.015) {
            std::cout << "Error at test " << test_no << " (ln) at x = " << x << " : " << " expected: " << b << " got: " << a << std::endl;
            status_code = 1;
        }
    }


    for (float x=-8;x<8;x+=2.5e-2) {
        test_no++;
        float a = (float)taylor::exp<fixed64>(fixed64(x));
        float b = std::exp(x);
        if (std::abs(a - b) > b*0.1) {
            std::cout << "Error at test " << test_no << " (exp) at x = " << x << " : " << " expected: " << b << " got: " << a << std::endl;
            status_code = 1;
        }
    }

    return status_code;
}
