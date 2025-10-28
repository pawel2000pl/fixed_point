#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <functional>
#include <iomanip>

#define TAYLOR_LOOP_COUNTER 1

#include "lib/fixedpoint.h"
#include "lib/taylormath.h"

template<typename T>
void test_function(const std::string& filename, double start, double stop, unsigned n, const std::function<T(T)> fun) {
    std::ofstream file(filename);
    file << std::fixed << std::setw(32) << std::setprecision(32);
    // initialize of needed
    fun(start); 
    fun((stop+start)/2);
    fun(stop);
    for (unsigned i=0;i<n;i++) {
        taylor::loop_counter = 0;
        double x = start + i * (stop - start) / n;
        const auto value = fun(x);
        file << x << "\t" << value << "\t" << taylor::loop_counter << "\n";
    }
    file.close();
}

int main() {

    unsigned n = 101;

    test_function<double>("plots/double_std_cos.txt", -10, 10, n, cos);
    test_function<double>("plots/double_std_sin.txt", -10, 10, n, sin);
    test_function<double>("plots/double_std_sqrt.txt", 0, 16, n, sqrt);
    test_function<double>("plots/double_std_asin.txt", -1, 1, n, asin);
    test_function<double>("plots/double_std_log.txt", 1e-3, 10, n, log);
    test_function<double>("plots/double_std_exp.txt", -10, 10, n, exp);

    test_function<double>("plots/double_taylor_cos.txt", -10, 10, n, taylor::cos<double>);
    test_function<double>("plots/double_taylor_sin.txt", -10, 10, n, taylor::sin<double>);
    test_function<double>("plots/double_taylor_sqrt.txt", 0, 16, n, taylor::sqrt<double>);
    test_function<double>("plots/double_taylor_asin.txt", -1, 1, n, taylor::asin<double>);
    test_function<double>("plots/double_taylor_log.txt", 1e-3, 10, n, taylor::log<double>);
    test_function<double>("plots/double_taylor_exp.txt", -10, 10, n, taylor::exp<double>);

    test_function<float>("plots/float_std_cos.txt", -10, 10, n, cos);
    test_function<float>("plots/float_std_sin.txt", -10, 10, n, sin);
    test_function<float>("plots/float_std_sqrt.txt", 0, 16, n, sqrt);
    test_function<float>("plots/float_std_asin.txt", -1, 1, n, asin);
    test_function<float>("plots/float_std_log.txt", 1e-3, 10, n, log);
    test_function<float>("plots/float_std_exp.txt", -10, 10, n, exp);

    test_function<float>("plots/float_taylor_cos.txt", -10, 10, n, taylor::cos<float>);
    test_function<float>("plots/float_taylor_sin.txt", -10, 10, n, taylor::sin<float>);
    test_function<float>("plots/float_taylor_sqrt.txt", 0, 16, n, taylor::sqrt<float>);
    test_function<float>("plots/float_taylor_asin.txt", -1, 1, n, taylor::asin<float>);
    test_function<float>("plots/float_taylor_log.txt", 1e-3, 10, n, taylor::log<float>);
    test_function<float>("plots/float_taylor_exp.txt", -10, 10, n, taylor::exp<float>);

    test_function<fixed32_s>("plots/fixed32_s_taylor_cos.txt", -10, 10, n, taylor::cos<fixed32_s>);
    test_function<fixed32_s>("plots/fixed32_s_taylor_sin.txt", -10, 10, n, taylor::sin<fixed32_s>);
    test_function<fixed32_s>("plots/fixed32_s_taylor_sqrt.txt", 0, 16, n, taylor::sqrt<fixed32_s>);
    test_function<fixed32_s>("plots/fixed32_s_taylor_asin.txt", -1, 1, n, taylor::asin<fixed32_s>);
    test_function<fixed32_s>("plots/fixed32_s_taylor_log.txt", 1e-3, 10, n, taylor::log<fixed32_s>);
    test_function<fixed32_s>("plots/fixed32_s_taylor_exp.txt", -10, 10, n, taylor::exp<fixed32_s>);

    test_function<fixed32_a>("plots/fixed32_a_taylor_cos.txt", -10, 10, n, taylor::cos<fixed32_a>);
    test_function<fixed32_a>("plots/fixed32_a_taylor_sin.txt", -10, 10, n, taylor::sin<fixed32_a>);
    test_function<fixed32_a>("plots/fixed32_a_taylor_sqrt.txt", 0, 16, n, taylor::sqrt<fixed32_a>);
    test_function<fixed32_a>("plots/fixed32_a_taylor_asin.txt", -1, 1, n, taylor::asin<fixed32_a>);
    test_function<fixed32_a>("plots/fixed32_a_taylor_log.txt", 1e-3, 10, n, taylor::log<fixed32_a>);
    test_function<fixed32_a>("plots/fixed32_a_taylor_exp.txt", -10, 10, n, taylor::exp<fixed32_a>);

    test_function<fixed64>("plots/fixed64_taylor_cos.txt", -10, 10, n, taylor::cos<fixed64>);
    test_function<fixed64>("plots/fixed64_taylor_sin.txt", -10, 10, n, taylor::sin<fixed64>);
    test_function<fixed64>("plots/fixed64_taylor_sqrt.txt", 0, 16, n, taylor::sqrt<fixed64>);
    test_function<fixed64>("plots/fixed64_taylor_asin.txt", -1, 1, n, taylor::asin<fixed64>);
    test_function<fixed64>("plots/fixed64_taylor_log.txt", 1e-3, 10, n, taylor::log<fixed64>);
    test_function<fixed64>("plots/fixed64_taylor_exp.txt", -10, 10, n, taylor::exp<fixed64>);

    return 0;
}
