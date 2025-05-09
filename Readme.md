# Library for fixed point operations with math module based on (mostly) taylor series.

Template dedicated for RISC-V IM.

## Usage

### Files

All library files can be found in `src/lib/`.
Just copy the files and include them.<br>
Additionally `taylormath` need to be linked with its `cpp` file.

### Usage in code

The template has the following prototype:
~~~
template<typename T, typename TC=typename make_fast_int<T>::type, unsigned frac_bits=sizeof(T)*4-1>
~~~

* `T` - integer type for calculations.
* `TC` - integer type for multiplication and division. By default it is `fast` type of the `T` (on 64-bits systems it is usually `int64` / `uint64`)
* `frac_bits` - number - where to put the fraction point.

There are predefinied types:

* Standard types

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

* Accurate types

    * signed

        * fixed8_a - signed 8 bit `T`, fast 16 bit type for `TC`, 3 fraction bits
        * fixed16_a - signed 16 bit `T`, fast 32 bit type for `TC`, 7 fraction bits
        * fixed32_a - signed 32 bit `T`, fast 64 bit type for `TC`, 15 fraction bits
        * fixed64_a - signed 64 bit `T`, fast 64 bit type for `TC`, 31 fraction bits

    * unsigned

        * ufixed8_a - unsigned 8 bit `T`, fast 16 bit type for `TC`, 3 fraction bits
        * ufixed16_a - unsigned 16 bit `T`, fast 32 bit type for `TC`, 7 fraction bits
        * ufixed32_a - unsigned 32 bit `T`, fast 64 bit type for `TC`, 15 fraction bits
        * ufixed64_a - unsigned 64 bit `T`, fast 64 bit type for `TC`, 31 fraction bits

* Simple types

    * signed

        * fixed8 - signed 8 bit `T`, 8 bit type for `TC`, 3 fraction bits
        * fixed16 - signed 16 bit `T`, 16 bit type for `TC`, 7 fraction bits
        * fixed32 - signed 32 bit `T`, 32 bit type for `TC`, 15 fraction bits
        * fixed64 - signed 64 bit `T`, 64 bit type for `TC`, 31 fraction bits

    * unsigned

        * ufixed8 - unsigned 8 bit `T`, 8 bit type for `TC`, 3 fraction bits
        * ufixed16 - unsigned 16 bit `T`, 16 bit type for `TC`, 7 fraction bits
        * ufixed32 - unsigned 32 bit `T`, 32 bit type for `TC`, 15 fraction bits
        * ufixed64 - unsigned 64 bit `T`, 64 bit type for `TC`, 31 fraction bits

### Problem with operators

Due to possible inconsistency of result type on calculations of various fixedpoints both operands should be the same type. But it is not something that you must remember because the compiler will remind you.<br>
The solution is to use casting, f.e.: 
~~~
fixed32 x = 1;
fixed64 y = 2.98;
std::cout << x + (fixed32)y << std::endl;
std::cout << (fixed64)x + y << std::endl;
~~~



## Test results

### Speed comparision (microseconds per 100000 operations on ESP32C3@160MHz)

<table><thead><tr><th>type</th><th>library</th><th>addition</th><th>subtraction</th><th>multiplication</th><th>division</th><th>sin</th><th>sqrt</th><th>asin</th><th>log</th><th>exp</th></tr></thead><tbody>
<tr><th>fixed32_s</th><th>taylormath</th><td>6980</td><td>6916</td><td>6918</td><td>61020</td><td>213165</td><td>89189</td><td>114103</td><td>368566</td><td>300974</td>

</tr>
<tr><th>fixed32_a</th><th>taylormath</th><td>6955</td><td>6923</td><td>15724</td><td>201886</td><td>320023</td><td>258279</td><td>171566</td><td>870242</td><td>397568</td>

</tr>
<tr><th>fixed64</th><th>taylormath</th><td>13252</td><td>13211</td><td>20760</td><td>205677</td><td>878301</td><td>404004</td><td>398262</td><td>2226945</td><td>1439338</td>

</tr>
<tr><th>float</th><th>cmath</th><td>101478</td><td>105844</td><td>163514</td><td>255338</td><td>2025239</td><td>344892</td><td>535353</td><td>1527860</td><td>2016053</td>

</tr>
<tr><th>double</th><th>cmath</th><td>118824</td><td>121313</td><td>288665</td><td>494619</td><td>3027907</td><td>615168</td><td>843381</td><td>2339772</td><td>2997068</td>

</tr>
<tr><th>float</th><th>taylormath</th><td>101498</td><td>105844</td><td>164147</td><td>254708</td><td>2708558</td><td>1979583</td><td>1467586</td><td>8793029</td><td>4190682</td>

</tr>
<tr><th>double</th><th>taylormath</th><td>118836</td><td>121306</td><td>289296</td><td>495243</td><td>5849447</td><td>8746185</td><td>7214242</td><td>35382185</td><td>10262032</td>

</tr>
</tbody></table>

<br>
Taylormath calculates until increasing accuracy is not possible, so the more accurate type, the more time is needed to calculate a function.

### Taylormath accuracy 

Differences with double and cmath as reference.<br>

![asin plot](plots/plot_asin.png)
![cos plot](plots/plot_cos.png)
![sin plot](plots/plot_sin.png)
![log plot](plots/plot_log.png)
![exp plot](plots/plot_exp.png)
![sqrt plot](plots/plot_sqrt.png)
