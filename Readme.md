# Library for fixed point operations with alternative specialized math modules.

Template dedicated for RISC-V 32 IM.<br>

There are two math modules:
* taylormath which is based mostly on taylor series
* polyapprox which is based on cubic Hermite interpolator

## Usage

### Files

All library files can be found in `src/lib/`.
Just copy the files and include them.<br>
Additionally `taylormath` need to be linked with its `cpp` file.
<br>
The library works with C++:
* std-c++23
* std-c++20
* std-c++17
* std-c++11 if you are not going to use constexpr functionalities (they works but not everywhere)

### Fixedpoint

The fixedpoint template has the following prototype:
~~~
template<typename T, typename TC=typename make_fast_int<T>::type, unsigned frac_bits=sizeof(T)*4-1> class fixedpoint;
~~~

* `T` - integer type for calculations.
* `TC` - integer type for multiplication and division. By default it is `fast` type of the `T` (on 64-bits systems it is usually `int64` / `uint64`)
* `frac_bits` - number - where to put the fraction point.

There are predefinied types:

* Standard types (the fastest TC with at least T size)

    * signed

        * fixed8 - signed 8 bit `T`, fast 8 bit type for `TC`, 3 fraction bits
        * fixed16 - signed 16 bit `T`, fast 16 bit type for `TC`, 7 fraction bits
        * fixed32 - signed 32 bit `T`, fast 32 bit type for `TC`, 15 fraction bits
        * fixed64 - signed 64 bit `T`, fast 64 bit type for `TC`, 31 fraction bits

    * unsigned

        * ufixed8 - unsigned 8 bit `T`, fast 8 bit type for `TC`, 3 fraction bits
        * ufixed16 - unsigned 16 bit `T`, fast 16 bit type for `TC`, 7 fraction bits
        * ufixed32 - unsigned 32 bit `T`, fast 32 bit type for `TC`, 15 fraction bits
        * ufixed64 - unsigned 64 bit `T`, fast 64 bit type for `TC`, 31 fraction bits

* Accurate types (TC two times larger than T - except [u]int64)

    * signed

        * fixed8_a - signed 8 bit `T`, fast 16 bit type for `TC`, 3 fraction bits
        * fixed16_a - signed 16 bit `T`, fast 32 bit type for `TC`, 7 fraction bits
        * fixed32_a - signed 32 bit `T`, fast 64 bit type for `TC`, 15 fraction bits
        * *fixed64_a - signed 64 bit `T`, fast 64 bit type for `TC`, 31 fraction bits*

    * unsigned

        * ufixed8_a - unsigned 8 bit `T`, fast 16 bit type for `TC`, 3 fraction bits
        * ufixed16_a - unsigned 16 bit `T`, fast 32 bit type for `TC`, 7 fraction bits
        * ufixed32_a - unsigned 32 bit `T`, fast 64 bit type for `TC`, 15 fraction bits
        * *ufixed64_a - unsigned 64 bit `T`, fast 64 bit type for `TC`, 31 fraction bits*

* Simple types (same size of T and TC)

    * signed

        * fixed8_s - signed 8 bit `T`, 8 bit type for `TC`, 3 fraction bits
        * fixed16_s - signed 16 bit `T`, 16 bit type for `TC`, 7 fraction bits
        * fixed32_s - signed 32 bit `T`, 32 bit type for `TC`, 15 fraction bits
        * fixed64_s - signed 64 bit `T`, 64 bit type for `TC`, 31 fraction bits

    * unsigned

        * ufixed8_s - unsigned 8 bit `T`, 8 bit type for `TC`, 3 fraction bits
        * ufixed16_s - unsigned 16 bit `T`, 16 bit type for `TC`, 7 fraction bits
        * ufixed32_s - unsigned 32 bit `T`, 32 bit type for `TC`, 15 fraction bits
        * ufixed64_s - unsigned 64 bit `T`, 64 bit type for `TC`, 31 fraction bits

* Other types (based on `std::size_t`)

    * signed

        * fixed_t - signed `std::size_t` for `T`, fast signed `std::size_t` for `TC`, `sizeof(std::size_t) * 4 - 1` fraction bits;

    * unsigned

        * ufixed_t - unsigned `std::size_t` for `T`, fast unsigned `std::size_t` for `TC`, `sizeof(std::size_t) * 4 - 1` fraction bits;

#### Conversions from IEEE754

 Results of conversion from IEEE754 might be incorrect due to reading the numbers binary. 
 Always check results on the new target.
 In case of any errors use FIXED_POINT_IEEE754_ALWAYS_MULTIPLICATE macro.

### Taylormath

Each template takes a type which is used for calculations. 
It can be deduced by a compiler, but to avoid mistakes and calculating on too accuracy (and slower) type 
it is advised to fill the template parameter, f.e:
~~~~
std::cout << taylor::exp<fixed32_f>(x) << "\n";
std::cout << taylor::asin<fixed32_f>(x) << "\n";
std::cout << taylor::cos<fixed32_f>(x) << "\n";
std::cout << taylor::sqrt<fixed32_f>(x) << "\n";
~~~~

Taylormath calculates until increasing accuracy is not possible, so the more accurate type, the more time is needed to calculate a function result.

### Polyapprox

This is a class which allows to create an approximation (functional). 
There are required one template parameter (`Storable`) which will be used for parameters storage and calculating the value of the approximation.<br>
The constructor / `fit` method / `create` static function takes the following parameters:
* `src` - functional of a source function
* `part_count` - number of divisions of the source function on a given range
* `range_min` - minimal value of the range
* `range_max` - maximal value of the range
* `dx` - step for deriverates (default 1e-3)

The constructor, `fit` method and `create` static function are templates. 
The only parameter is a type which will be used for calculating parameters for the approximation.

## Test results

### Speed comparision (microseconds per 100000 operations on ESP32C3@160MHz)
Read `ArduinoFixedPointTest/tests.h` to see details.

<table><thead><tr><th>type</th><th>library</th><th>addition</th><th>subtraction</th><th>multiplication</th><th>division</th><th>sin</th><th>sqrt</th><th>asin</th><th>log</th><th>exp</th></tr></thead><tbody>
<tr><th>fixed32_s</th><th>taylormath</th><td>6981</td><td>6917</td><td>10698</td><td>26436</td><td>169572</td><td>143301</td><td>522276</td><td>453157</td><td>267850</td>

</tr>
<tr><th>fixed32_a</th><th>taylormath</th><td>6956</td><td>6923</td><td>13846</td><td>105995</td><td>284164</td><td>470334</td><td>788120</td><td>1136635</td><td>357140</td>

</tr>
<tr><th>fixed64</th><th>taylormath</th><td>13240</td><td>13212</td><td>20769</td><td>114558</td><td>841901</td><td>772750</td><td>1977503</td><td>2864429</td><td>1441286</td>

</tr>
<tr><th>fixed32_s</th><th>polyapprox</th><td>6963</td><td>6924</td><td>10696</td><td>26434</td><td>53140</td><td>52686</td><td>54072</td><td>52684</td><td>52913</td>

</tr>
<tr><th>fixed32_a</th><th>polyapprox</th><td>6957</td><td>6923</td><td>13845</td><td>105997</td><td>71078</td><td>69839</td><td>71054</td><td>69783</td><td>71193</td>

</tr>
<tr><th>fixed64</th><th>polyapprox</th><td>13250</td><td>13212</td><td>20766</td><td>114554</td><td>108361</td><td>108048</td><td>109319</td><td>107843</td><td>109227</td>

</tr>
<tr><th>float</th><th>cmath</th><td>101470</td><td>105847</td><td>87411</td><td>134307</td><td>2048567</td><td>682605</td><td>2684685</td><td>2190272</td><td>2035009</td>

</tr>
<tr><th>double</th><th>cmath</th><td>118840</td><td>121316</td><td>150957</td><td>256267</td><td>3058473</td><td>1248267</td><td>4236754</td><td>3361766</td><td>3026707</td>

</tr>
<tr><th>float</th><th>taylormath</th><td>101484</td><td>105847</td><td>87406</td><td>134307</td><td>2712767</td><td>3679105</td><td>7189580</td><td>12053340</td><td>4208784</td>

</tr>
<tr><th>double</th><th>taylormath</th><td>118830</td><td>121316</td><td>150958</td><td>256267</td><td>5858965</td><td>16485806</td><td>35059986</td><td>48714662</td><td>10290869</td>

</tr>
</tbody></table>


### Taylormath and Polyapprox accuracy 

Differences with double and cmath as reference.<br>
Polyapprox uses 31-points in test cases.<br>

Taylormath                 |  Polyapprox
:-------------------------:|:-------------------------:
![asin taylor plot](plots/plot_taylor_asin.png) | ![asin polyapprox plot](plots/plot_approx_asin.png)
![cos taylor plot](plots/plot_taylor_cos.png) | ![cos polyapprox plot](plots/plot_approx_cos.png)
![sin taylor plot](plots/plot_taylor_sin.png) | ![sin polyapprox plot](plots/plot_approx_sin.png)
![log taylor plot](plots/plot_taylor_log.png) | ![log polyapprox plot](plots/plot_approx_log.png)
![exp taylor plot](plots/plot_taylor_exp.png) | ![exp polyapprox plot](plots/plot_approx_exp.png)
![sqrt taylor plot](plots/plot_taylor_sqrt.png) | ![sqrt polyapprox plot](plots/plot_approx_sqrt.png)

### Taylormath iterations 

![asin taylor iterations plot](plots/plot_taylor_asin_iterations.png)
![cos taylor iterations plot](plots/plot_taylor_cos_iterations.png)
![sin taylor iterations plot](plots/plot_taylor_sin_iterations.png)
![log taylor iterations plot](plots/plot_taylor_log_iterations.png)
![exp taylor iterations plot](plots/plot_taylor_exp_iterations.png)
![sqrt taylor iterations plot](plots/plot_taylor_sqrt_iterations.png)

