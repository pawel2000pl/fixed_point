#pragma once

#include<Arduino.h>
#include<cmath>
#include<functional>
#include<functional>

#include "lib/fixedpoint.h"
#include "lib/taylormath.h"


volatile float result_dump = 0;

template<typename T> 
T add_test(unsigned n) {
  T result = 0;
  for (unsigned i=0;i<n;i++) {
    result += (signed char)i;
    asm volatile("" : "+r"(result));
  }
  return result;
}

template<typename T> 
T sub_test(unsigned n) {
  T result = 0;
  for (unsigned i=0;i<n;i++) {
    result -= (signed char)i;
    asm volatile("" : "+r"(result));
  }
  return result;
}

template<typename T> 
T prod_test(unsigned n) {
  T result = 0.001f;
  T mul1 = 1.01f;
  T mul2 = 0.9901f;
  for (unsigned i=0;i<n;i++) {
    result *= mul1;
    result *= mul2;
    asm volatile("" : "+r"(result));
  }
  return result;
}


template<typename T> 
T quot_test(unsigned n) {
  T result = 1000;
  T div1 = 1.01f;
  T div2 = 0.9901f;
  for (unsigned i=0;i<n;i++) {
    result /= div1;
    result /= div2;
    asm volatile("" : "+r"(result));
  }
  return result;
}


template<typename T> 
T math_test(T start, T stop, T step, const std::function<T(T)>& fun) {
  T result = 0;
  bool direction = 0;
  for (T i=start;i<stop;i+=step) {
    if (direction)
      result -= fun(T(i));
    else
      result += fun(T(i));
    if (std::abs(result) > 1000) {
      direction = !direction;
      result /= result * 0.01;
    }
  }
  return result;
}


unsigned long long int measure_time(const std::function<float()>& fun) {
  unsigned long long int t0 = micros();
  result_dump = fun();
  return (unsigned long long int)micros() - t0;
}


template<typename T>
void all_tests_taylor(unsigned n) {

  unsigned long long int add = measure_time([=](){return (float)add_test<T>(n);});
  unsigned long long int sub = measure_time([=](){return (float)sub_test<T>(n);});
  unsigned long long int prod = measure_time([=](){return (float)prod_test<T>(n);});
  unsigned long long int quot = measure_time([=](){return (float)quot_test<T>(n);});

  unsigned long long int tsin = measure_time([=](){return (float)math_test<T>(-5, 5, T(10)/n, &taylor::sin<T>);});
  unsigned long long int tsqrt = measure_time([=](){return (float)math_test<T>(0, 5, T(10)/n, &taylor::sqrt<T>);});
  unsigned long long int tasin = measure_time([=](){return (float)math_test<T>(-0.999, 0.999, T(10)/n, &taylor::asin<T>);});
  unsigned long long int tlog = measure_time([=](){return (float)math_test<T>(0.001, 7, T(10)/n, &taylor::log<T>);});
  unsigned long long int texp = measure_time([=](){return (float)math_test<T>(-5, 5, T(10)/n, &taylor::exp<T>);});

  Serial.printf( 
    "<td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td>\n\r",
    add, sub, prod, quot, tsin, tsqrt, tasin, tlog, texp
  );
}


template<typename T>
void all_tests_std(unsigned n) {

  unsigned long long int add = measure_time([=](){return (float)add_test<T>(n);});
  unsigned long long int sub = measure_time([=](){return (float)sub_test<T>(n);});
  unsigned long long int prod = measure_time([=](){return (float)prod_test<T>(n);});
  unsigned long long int quot = measure_time([=](){return (float)quot_test<T>(n);});

  unsigned long long int tsin = measure_time([=](){return (float)math_test<T>(-5, 5, T(10)/n, [](T x){return sin(x);});});
  unsigned long long int tsqrt = measure_time([=](){return (float)math_test<T>(0, 5, T(10)/n, [](T x){return sqrt(x);});});
  unsigned long long int tasin = measure_time([=](){return (float)math_test<T>(-0.999, 0.999, T(10)/n, [](T x){return asin(x);});});
  unsigned long long int tlog = measure_time([=](){return (float)math_test<T>(0.001, 7, T(10)/n, [](T x){return log(x);});});
  unsigned long long int texp = measure_time([=](){return (float)math_test<T>(-5, 5, T(10)/n, [](T x){return exp(x);});});

  Serial.printf( 
    "<td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td>\n\r",
    add, sub, prod, quot, tsin, tsqrt, tasin, tlog, texp
  );
}




