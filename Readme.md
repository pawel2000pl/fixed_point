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
<tr><th>fixed32_s</th><th>taylormath</th><td>6997</td><td>6923</td><td>6291</td><td>38998</td><td>502843</td><td>78624</td><td>207197</td><td>349685</td><td>899408</td>

</tr>
<tr><th>fixed32_a</th><th>taylormath</th><td>6956</td><td>6923</td><td>15730</td><td>201884</td><td>634943</td><td>260499</td><td>249677</td><td>1379052</td><td>999560</td>

</tr>
<tr><th>fixed64</th><th>taylormath</th><td>13248</td><td>13212</td><td>20762</td><td>206311</td><td>888863</td><td>389884</td><td>396304</td><td>2069122</td><td>1471446</td>

</tr>
<tr><th>float</th><th>cmath</th><td>101478</td><td>105839</td><td>163524</td><td>255334</td><td>2025276</td><td>344894</td><td>535362</td><td>1527878</td><td>2016057</td>

</tr>
<tr><th>double</th><th>cmath</th><td>118832</td><td>121308</td><td>288677</td><td>494622</td><td>3026964</td><td>614710</td><td>843229</td><td>2339152</td><td>2996110</td>

</tr>
<tr><th>float</th><th>taylormath</th><td>101476</td><td>105845</td><td>163518</td><td>255343</td><td>3190664</td><td>1839418</td><td>1586320</td><td>9621808</td><td>4867242</td>

</tr>
<tr><th>double</th><th>taylormath</th><td>118837</td><td>121314</td><td>288671</td><td>494622</td><td>6843760</td><td>8243932</td><td>8178865</td><td>37497165</td><td>11667931</td>

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
