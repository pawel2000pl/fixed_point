#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>

#include "lib/fixedpoint.h"

int status_code = 0;
unsigned long long test_no = 0;

template<typename FP>
void compare_test_plus_minus(float lo, float hi, float inc, float epsilon) {

    FP a, b, c;
    float fa, fb, fc;

    for (float ia=lo;ia<hi;ia+=inc) {
        for (float ib=lo;ib<hi;ib+=inc) {
            test_no++;
            a = ia;
            b = ib;
            fa = ia;
            fb = ib;
            c = a + b;
            fc = fa + fb;
            if (std::abs(fc - (float)c) > epsilon) {
                std::cout << "Error at test [+] " << test_no << " of " << typeid(FP).name() << ": " << ia << " + " << ib << " expected: " << fc << " got: " << c << std::endl;
                status_code = 1;
            }
            c = a - b;
            fc = fa - fb;
            if (std::abs(fc - (float)c) > epsilon) {                
                std::cout << "Error at test [-] " << test_no << " of " << typeid(FP).name() << ": " << ia << " - " << ib << " expected: " << fc << " got: " << c << std::endl;
                status_code = 1;
            }
        }
    }
}


template<typename FP>
void compare_test_multiplication(float lo, float hi, float inc, float epsilon, float epsilon_mult) {

    FP a, b, c;
    float fa, fb, fc;

    for (float ia=lo;ia<hi;ia+=inc) {
        for (float ib=lo;ib<hi;ib+=inc) {
            test_no++;
            a = ia;
            b = ib;
            fa = ia;
            fb = ib;

            c = a * b;
            fc = fa * fb;
            if (std::abs(fc - (float)c) > std::max(epsilon, std::abs(fc)*epsilon_mult)) {
                std::cout << "Error at test [*]" << test_no << " of " << typeid(FP).name() << ": " << ia << " * " << ib << " expected: " << fc << " got: " << c << std::endl;
                status_code = 1;
            }
        }
    }
}


template<typename FP>
void compare_test_division(float lo, float hi, float inc, float epsilon, float epsilon_mult) {

    FP a, b, c;
    float fa, fb, fc;

    for (float ia=lo;ia<hi;ia+=inc) {
        for (float ib=lo;ib<hi;ib+=inc) {
            test_no++;
            a = ia;
            b = ib;
            fa = ia;
            fb = ib;

            if (std::abs(ib) > epsilon) {
                c = a / b;
                fc = fa / fb;
                if (std::abs(fc - (float)c) > std::max(epsilon, std::abs(fc)*epsilon_mult)) {
                    std::cout << "Error at test [/]" << test_no << " of " << typeid(FP).name() << ": " << ia << " / " << ib << " expected: " << fc << " got: " << c << std::endl;
                    status_code = 1;
                }
            }
        }
    }
}


int main() {

    std::cout << std::fixed << std::setw(8) << std::setprecision(8);

    for (float k=1;k<1000;k*=10) {
        compare_test_plus_minus<fixedpoint<int, int, 20>>(-3e-2*k, 3e-2*k, 1e-4*k, 2e-6);
        compare_test_multiplication<fixedpoint<int, int, 20>>(-3e-2*k, 3e-2*k, 1e-4*k, 2e-4*k, 0.2);
        compare_test_division<fixedpoint<int, int, 20>>(-3e-2*k, 3e-2*k, 1e-4*k, 5e-3, 0.2);

        compare_test_plus_minus<fixedpoint<int, long long int, 20>>(-3e-2*k, 3e-2*k, 1e-4*k, 2e-6);
        compare_test_multiplication<fixedpoint<int, long long int, 20>>(-3e-2*k, 3e-2*k, 1e-4*k, 2e-6*k, 1e-3);
        compare_test_division<fixedpoint<int, long long int, 20>>(-3e-2*k, 3e-2*k, 1e-4*k, 1e-3, 8e-3);

        compare_test_plus_minus<fixedpoint<long long int, long long int, 20>>(-3e-2*k, 3e-2*k, 1e-4*k, 2e-6);
        compare_test_multiplication<fixedpoint<long long int, long long int, 20>>(-3e-2*k, 3e-2*k, 1e-4*k, 2e-4*k, 0.2);
        compare_test_division<fixedpoint<long long int, long long int, 20>>(-3e-2*k, 3e-2*k, 1e-4*k, 5e-3, 0.2);


        compare_test_plus_minus<fixedpoint<int, int, 10>>(-3e-2*k, 3e-2*k, 1e-4*k, 2e-3);
        compare_test_multiplication<fixedpoint<int, int, 10>>(-3e-2*k, 3e-2*k, 1e-4*k, 2e-2*k, 0.2);
        compare_test_division<fixedpoint<int, int, 10>>(-3e-2*k, 3e-2*k, 1e-4*k, 5e-1, 0.2);

        compare_test_plus_minus<fixedpoint<int, long long int, 10>>(-3e-2*k, 3e-2*k, 1e-4*k, 2e-3);
        compare_test_multiplication<fixedpoint<int, long long int, 10>>(-3e-2*k, 3e-2*k, 1e-4*k, 2e-3*k, 1e-3);
        compare_test_division<fixedpoint<int, long long int, 10>>(-3e-2*k, 3e-2*k, 1e-4*k, 1, 8e-3);

        compare_test_plus_minus<fixedpoint<long long int, long long int, 10>>(-3e-2*k, 3e-2*k, 1e-4*k, 2e-3);
        compare_test_multiplication<fixedpoint<long long int, long long int, 10>>(-3e-2*k, 3e-2*k, 1e-4*k, 2e-2*k, 0.2);
        compare_test_division<fixedpoint<long long int, long long int, 10>>(-3e-2*k, 3e-2*k, 1e-4*k, 5e-1, 0.2);
    }


    return status_code;
}
