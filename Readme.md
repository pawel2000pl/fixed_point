# Library for fixed point operations with math module based on (mostly) taylor series.

Template dedicated for RISC-V 32 IM.

## Usage

### Files

All library files can be found in `src/lib/`.
Just copy the files and include them.<br>
Additionally `taylormath` need to be linked with its `cpp` file.

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



## Test results

### Speed comparision (microseconds per 100000 operations on ESP32C3@160MHz)

STARTING TESTS

<table><thead><tr><th>type</th><th>library</th><th>addition</th><th>subtraction</th><th>multiplication</th><th>division</th><th>sin</th><th>sqrt</th><th>asin</th><th>log</th><th>exp</th></tr></thead><tbody>
<tr><th>fixed32_s</th><th>taylormath</th><td>6982</td><td>6917</td><td>6298</td><td>61652</td><td>189845</td><td>85509</td><td>113827</td><td>360010</td><td>300977</td>

</tr>
<tr><th>fixed32_a</th><th>taylormath</th><td>6956</td><td>6922</td><td>15729</td><td>201253</td><td>303803</td><td>254862</td><td>170733</td><td>861728</td><td>397566</td>

</tr>
<tr><th>fixed64</th><th>taylormath</th><td>13247</td><td>13211</td><td>20761</td><td>206308</td><td>835902</td><td>399448</td><td>396151</td><td>2217824</td><td>1437792</td>

</tr>
<tr><th>float</th><th>cmath</th><td>101477</td><td>105844</td><td>164146</td><td>254705</td><td>2025247</td><td>344892</td><td>535360</td><td>1527857</td><td>2016053</td>

</tr>
<tr><th>double</th><th>cmath</th><td>118830</td><td>121306</td><td>289300</td><td>495248</td><td>3026929</td><td>614699</td><td>843223</td><td>2339121</td><td>2996070</td>

</tr>
<tr><th>float</th><th>taylormath</th><td>101490</td><td>105844</td><td>163517</td><td>255330</td><td>2686019</td><td>1839398</td><td>1434164</td><td>8438956</td><td>4190184</td>

</tr>
<tr><th>double</th><th>taylormath</th><td>118830</td><td>121313</td><td>288667</td><td>494616</td><td>5820614</td><td>8243824</td><td>7006465</td><td>34110313</td><td>10240876</td>

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
