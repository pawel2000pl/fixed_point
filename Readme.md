# Library for fixed point operations with math module based on (mostly) taylor series.

Template dedicated for RISC-V 32 IM.

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

### Usage in code

The template has the following prototype:
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

### Problem with operators

Due to possible inconsistency of result type on calculations of various fixedpoints both operands should be the same type. But it is not something that you must remember because the compiler will remind you.<br>
The solution is to use casting, f.e.: 
~~~
fixed32 x = 7.35;
fixed64 y = 21.37;
std::cout << x + (fixed32)y << std::endl;
std::cout << (fixed64)x + y << std::endl;
~~~

### Conversions from IEEE754
All conversions from floats works by multiplying float / double variable by some constant and then casting it to an integer type. It could be faster by using one of the following functions:

* from_ieee754
* from_float
* from_double

all of them might be faster or slower - it depends on the target platform. **Results of these functions could be also incorrect so always check results on the new target.** Although some assertions were made to avoid errors and thus in case of detection of an unsupported float/double format, these functions will use "standard" multiplication.


## Test results

### Speed comparision (microseconds per 100000 operations on ESP32C3@160MHz)

<table><thead><tr><th>type</th><th>library</th><th>addition</th><th>subtraction</th><th>multiplication</th><th>division</th><th>sin</th><th>sqrt</th><th>asin</th><th>log</th><th>exp</th></tr></thead><tbody>
<tr><th>fixed32_s</th><th>taylormath</th><td>6980</td><td>6917</td><td>6921</td><td>38997</td><td>187894</td><td>78628</td><td>113231</td><td>343401</td><td>294970</td>

</tr>
<tr><th>fixed32_a</th><th>taylormath</th><td>6956</td><td>6924</td><td>15729</td><td>201254</td><td>306980</td><td>256226</td><td>171002</td><td>865186</td><td>394723</td>

</tr>
<tr><th>fixed64</th><th>taylormath</th><td>13268</td><td>13211</td><td>20755</td><td>205680</td><td>836382</td><td>384171</td><td>392481</td><td>2007695</td><td>1436104</td>

</tr>
<tr><th>float</th><th>cmath</th><td>101481</td><td>105843</td><td>164146</td><td>254703</td><td>2026087</td><td>336237</td><td>533432</td><td>1527398</td><td>2015375</td>

</tr>
<tr><th>double</th><th>cmath</th><td>118823</td><td>121311</td><td>289295</td><td>495235</td><td>3022846</td><td>613325</td><td>841096</td><td>2339528</td><td>2995495</td>

</tr>
<tr><th>float</th><th>taylormath</th><td>101481</td><td>105843</td><td>164145</td><td>254704</td><td>2690285</td><td>1834694</td><td>1432962</td><td>8418461</td><td>4189229</td>

</tr>
<tr><th>double</th><th>taylormath</th><td>118835</td><td>121304</td><td>289303</td><td>495234</td><td>5823425</td><td>8232447</td><td>7001769</td><td>34081771</td><td>10238240</td>

</tr>
</tbody></table>

<br>
Taylormath calculates until increasing accuracy is not possible, so the more accurate type, the more time is needed to calculate a function result.

### Taylormath accuracy 

Differences with double and cmath as reference.<br>

![asin plot](plots/plot_asin.png)
![cos plot](plots/plot_cos.png)
![sin plot](plots/plot_sin.png)
![log plot](plots/plot_log.png)
![exp plot](plots/plot_exp.png)
![sqrt plot](plots/plot_sqrt.png)
