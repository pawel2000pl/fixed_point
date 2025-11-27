#pragma once

#include<Arduino.h>
#include<cmath>
#include<functional>

#include "lib/fixedpoint.h"
#include "lib/taylormath.h"
#include "lib/polyapprox.h"


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
  asm volatile("" : "+r"(mul1));
  T mul2 = 0.07907690907008446f;
  asm volatile("" : "+r"(mul2));
  for (unsigned i=0;i<n;i++) {
    result *= ((signed char)i) ? mul1 : mul2;
    asm volatile("" : "+r"(result));
  }
  return result;
}


template<typename T> 
T quot_test(unsigned n) {
  T result = 1000;
  T div1 = 1.01f;
  asm volatile("" : "+r"(div1));
  T div2 = 0.07907690907008446f;
  asm volatile("" : "+r"(div2));
  for (unsigned i=0;i<n;i++) {
    result /= ((signed char)i) ? div1 : div2;
    asm volatile("" : "+r"(result));
  }
  return result;
}


template<typename T> 
T math_test(T start, T stop, unsigned n, const std::function<T(T)>& fun) {
  T span = stop - start;
  T step = T(span) * T(0.09407208683835973f);
  T result = 0;
  T x=start;
  bool direction = 0;
  for (int i=0;i<n;i++) {
    x+=step;
    if (x>=stop) x -= span;
    if (direction)
      result -= fun(T(x));
    else
      result += fun(T(x));
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

  // initialization (if needed)
  taylor::sin<T>(5);
  taylor::sqrt<T>(5);
  taylor::asin<T>(0.5f);
  taylor::log<T>(0.5f);
  taylor::exp<T>(10);

  unsigned long long int tsin = measure_time([=](){return (float)math_test<T>(-5, 5, n, &taylor::sin<T>);});
  unsigned long long int tsqrt = measure_time([=](){return (float)math_test<T>(0, 5, n, &taylor::sqrt<T>);});
  unsigned long long int tasin = measure_time([=](){return (float)math_test<T>(-0.999, 0.999, n, &taylor::asin<T>);});
  unsigned long long int tlog = measure_time([=](){return (float)math_test<T>(0.001, 7, n, &taylor::log<T>);});
  unsigned long long int texp = measure_time([=](){return (float)math_test<T>(-5, 5, n, &taylor::exp<T>);});

  Serial.printf( 
    "<td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td>\n\r",
    add, sub, prod, quot, tsin, tsqrt, tasin, tlog, texp
  );
}


template<typename T>
void all_tests_polyapprox(unsigned n) {

  unsigned long long int add = measure_time([=](){return (float)add_test<T>(n);});
  unsigned long long int sub = measure_time([=](){return (float)sub_test<T>(n);});
  unsigned long long int prod = measure_time([=](){return (float)prod_test<T>(n);});
  unsigned long long int quot = measure_time([=](){return (float)quot_test<T>(n);});

  // initialization (if needed)
  unsigned m = 31;

  PolyApprox<T> ap_sin = PolyApprox<T>::template create<double>([](double x) {return sin(x);}, m, -5, 5);
  PolyApprox<T> ap_sqrt = PolyApprox<T>::template create<double>([](double x) {return sqrt(x);}, m, 0, 5);
  PolyApprox<T> ap_asin = PolyApprox<T>::template create<double>([](double x) {return asin(x);}, m, -1, 1);
  PolyApprox<T> ap_log = PolyApprox<T>::template create<double>([](double x) {return log(x);}, m, 0.001f, 7);
  PolyApprox<T> ap_exp = PolyApprox<T>::template create<double>([](double x) {return exp(x);}, m, -5, 5);

  unsigned long long int tsin = measure_time([=](){return (float)math_test<T>(-5, 5, n, ap_sin);});
  unsigned long long int tsqrt = measure_time([=](){return (float)math_test<T>(0, 5, n, ap_sqrt);});
  unsigned long long int tasin = measure_time([=](){return (float)math_test<T>(-0.999, 0.999, n, ap_asin);});
  unsigned long long int tlog = measure_time([=](){return (float)math_test<T>(0.001, 7, n, ap_log);});
  unsigned long long int texp = measure_time([=](){return (float)math_test<T>(-5, 5, n, ap_exp);});

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

  // initialization (if needed)
  sin((T)5);
  sqrt((T)5);
  asin((T)0.5f);
  log((T)0.5f);
  exp((T)10);

  unsigned long long int tsin = measure_time([=](){return (float)math_test<T>(-5, 5, n, [](T x){return sin(x);});});
  unsigned long long int tsqrt = measure_time([=](){return (float)math_test<T>(0, 5, n, [](T x){return sqrt(x);});});
  unsigned long long int tasin = measure_time([=](){return (float)math_test<T>(-0.999, 0.999, n, [](T x){return asin(x);});});
  unsigned long long int tlog = measure_time([=](){return (float)math_test<T>(0.001, 7, n, [](T x){return log(x);});});
  unsigned long long int texp = measure_time([=](){return (float)math_test<T>(-5, 5, n, [](T x){return exp(x);});});

  Serial.printf( 
    "<td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%llu</td>\n\r",
    add, sub, prod, quot, tsin, tsqrt, tasin, tlog, texp
  );
}




