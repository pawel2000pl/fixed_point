/****************************************************************************************

                                       MIT License

               Copyright (c) 2025 Pawel Bielecki [pbielecki2000@gmail.com]

       Permission is hereby granted, free of charge, to any person obtaining a copy
      of this software and associated documentation files (the "Software"), to deal
       in the Software without restriction, including without limitation the rights
        to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
          copies of the Software, and to permit persons to whom the Software is
                 furnished to do so, subject to the following conditions:

      The above copyright notice and this permission notice shall be included in all
                     copies or substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
         IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
       FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
          AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
      LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
      OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
                                        SOFTWARE.

****************************************************************************************/


#ifndef FIXED_POINT
#define FIXED_POINT

#include <limits>
#include <cstddef>
#include <cstdint>
#include <typeindex>
#include <type_traits>


// T - type for data storage
// TC - type used for calculating products and quotients (best is double-sized T)
// frac_bits - count of fraction bits (bits after point)
template<typename T, typename TC, unsigned frac_bits> class fixedpoint;


namespace fixedpoint_helpers {

    template<typename T>
    struct make_fast_int {
        using type = T;
        static_assert(std::numeric_limits<T>::is_integer, "T must be an integer type.");
    };

    template<typename T>
    struct make_least_int {
        using type = T;
        static_assert(std::numeric_limits<T>::is_integer, "T must be an integer type.");
    };

    #define __MAKE_INT_MAPPING(N, PREFIX)                                \
        template<>                                                       \
        struct make_##PREFIX##_int<std::int##N##_t> {                    \
            using type = std::int_##PREFIX##N##_t;                       \
        };                                                               \
        template<>                                                       \
        struct make_##PREFIX##_int<std::uint##N##_t> {                   \
            using type = std::uint_##PREFIX##N##_t;                      \
        };

    #define __MAKE_INT_TRAITS(PREFIX) \
        __MAKE_INT_MAPPING(8, PREFIX) \
        __MAKE_INT_MAPPING(16, PREFIX) \
        __MAKE_INT_MAPPING(32, PREFIX) \
        __MAKE_INT_MAPPING(64, PREFIX)

    __MAKE_INT_TRAITS(fast)
    __MAKE_INT_TRAITS(least)

    #undef __MAKE_INT_TRAITS
    #undef __MAKE_INT_MAPPING

    template<typename U>
    struct is_fixedpoint : std::false_type {};
    
    template<typename T, typename TC, unsigned frac_bits>
    struct is_fixedpoint<fixedpoint<T, TC, frac_bits>> : std::true_type {};

    template<typename T, int default_accuracy>
    struct static_accuracy {
        private:
            template<typename U> static constexpr typename std::enable_if<std::is_integral<U>::value, int>::type test(bool) {return 0;}
            template<typename U> static constexpr typename std::enable_if<std::is_floating_point<U>::value, int>::type test(int) {return default_accuracy;}
            template<typename U> static constexpr typename std::enable_if<is_fixedpoint<U>::value, int>::type test(long) {return U::fraction_bits;}
        public:
            static constexpr const int value = test<T>(0);
    };


    template<typename T, int value>
    constexpr static T static_signed_shl(T x) {
        return value ? ((value >= 0) ? (x << value) : (x >> (-value))) : x;
    }


    #if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    using is_little_endian = std::integral_constant<bool, __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__>;
    #else
    using is_little_endian = std::integral_constant<bool, false>;
    #endif

    #if defined(__cplusplus) && __cplusplus == __ORDER_LITTLE_ENDIAN__
    using cpp_version = std::integral_constant<long, __cplusplus / 100>;
    #else
    using cpp_version = std::integral_constant<long, 0>;
    #endif

    template<typename F, typename I>
    struct Union2 {
        constexpr Union2(F x) : data{.x{x}} {}
        constexpr Union2(I i) : data{.i{i}} {}
        union {F x; I i;} data;
    };

    template<typename F, int expected_size>
    struct float_to_fraction_use_multiplication : std::integral_constant<bool, std::numeric_limits<F>::is_iec559 && sizeof(F) == expected_size && is_little_endian::value && (cpp_version::value > 11)> {};

    template<typename R, typename F, unsigned expected_size, unsigned fraction, unsigned exponents, int accuracy, typename int_buf = typename std::conditional<fraction <= 32, std::uint32_t, std::uint64_t>::type>
    static constexpr typename std::enable_if<float_to_fraction_use_multiplication<F, expected_size>::value, R>::type
    buf_from_ieee754_generic(Union2<F, typename std::make_unsigned<int_buf>::type> input) {
        using u_int_buf = typename std::make_unsigned<int_buf>::type;
        using s_int_buf = typename std::make_signed<int_buf>::type;
        using exp_mask = std::integral_constant<u_int_buf, (((u_int_buf)1 << exponents) - 1) << fraction>;
        using exp_offset = std::integral_constant<s_int_buf, ((s_int_buf)1 << (exponents-1)) - 1 + fraction - accuracy>;
        using frac_one = std::integral_constant<u_int_buf, ((u_int_buf)1 << fraction)>;
        using frac_mask = std::integral_constant<u_int_buf, ((u_int_buf)1 << fraction) - 1>;
        using sign_mask = std::integral_constant<u_int_buf, (u_int_buf)1 << (fraction+exponents)>;
        s_int_buf exp_bits = (input.data.i & exp_mask::value) >> fraction;
        u_int_buf y = (input.data.i & frac_mask::value) | (exp_bits ? frac_one::value : 0);
        s_int_buf exp = exp_offset::value - exp_bits;
        R z = (exp < 0) ? y << (-exp) : y >> (exp);
        return (input.data.i & sign_mask::value) ? -z : z;
    }

    template<typename R, typename F, unsigned expected_size, unsigned fraction, unsigned exponents, int accuracy, typename int_buf = typename std::conditional<fraction <= 32, std::uint32_t, std::uint64_t>::type>
    static constexpr typename std::enable_if<!float_to_fraction_use_multiplication<F, expected_size>::value, R>::type
    buf_from_ieee754_generic(F x) {
        return x * ((R)1 << accuracy);
    }

    template<typename R, int accuracy>
    static constexpr R buf_from_ieee754(float x) {
        return buf_from_ieee754_generic<R, float, 4, 23, 8, accuracy, std::uint32_t>(x);
    }

    template<typename R, int accuracy>
    static constexpr R buf_from_ieee754(double x) {
        return buf_from_ieee754_generic<R, double, 8, 52, 11, accuracy, std::uint64_t>(x);
    }


    template<typename T, typename RESULT_TYPE, int offset=0>
    constexpr static typename std::enable_if<std::is_floating_point<T>::value, RESULT_TYPE>::type 
    make_buf(T x) noexcept {        
        return buf_from_ieee754<RESULT_TYPE, offset>(x);
    }


    template<typename T, typename RESULT_TYPE, int offset=0>
    constexpr static typename std::enable_if<is_fixedpoint<T>::value, RESULT_TYPE>::type
    make_buf(T x) noexcept {
        using true_offset = std::integral_constant<int, offset - (int)T::fraction_bits>;
        return static_signed_shl<RESULT_TYPE, true_offset::value>(x.getBuf());
    }


    template<typename T, typename RESULT_TYPE, int offset=0>
    constexpr static typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, RESULT_TYPE>::type
    make_buf(T x) noexcept {
        return static_signed_shl<RESULT_TYPE, offset>(x);
    }

    static constexpr int max(int a, int b) {
        return (a > b) ? a : b;
    }

    static constexpr int min(int a, int b) {
        return (a < b) ? a : b;
    }

    template<typename T, typename U>
    struct copy_signed : std::conditional<std::is_signed<T>::value, typename std::make_signed<U>::type, typename std::make_unsigned<U>::type> {};

    template<bool sign, typename U>
    struct set_sign : std::conditional<sign, typename std::make_signed<U>::type, typename std::make_unsigned<U>::type> {};


    using fixed_t = fixedpoint<std::make_signed<std::size_t>::type, fixedpoint_helpers::make_fast_int<typename std::make_signed<std::size_t>::type>::type, sizeof(std::size_t) * 4 - 1>;

    template<typename T>
    struct as_fixed : std::conditional<is_fixedpoint<T>::value, T, typename std::conditional<std::is_integral<T>::value, fixedpoint<T, T, 0>, fixed_t>::type>::type {};

    template<typename A, typename B>
    struct result_type {
        constexpr const static bool is_signed = std::is_signed<A>::value || std::is_signed<B>::value;
        using a_fixed = as_fixed<A>;
        using b_fixed = as_fixed<B>;
        using larger_base_type = typename set_sign<is_signed, typename std::conditional<(sizeof(typename a_fixed::BUF_TYPE) > sizeof(typename b_fixed::BUF_TYPE)), typename a_fixed::BUF_TYPE, typename b_fixed::BUF_TYPE>::type>::type;
        using larger_operational_type = typename set_sign<is_signed, typename std::conditional<(sizeof(typename a_fixed::CALCULATE_TYPE) > sizeof(typename b_fixed::CALCULATE_TYPE)), typename a_fixed::CALCULATE_TYPE, typename b_fixed::CALCULATE_TYPE>::type>::type;
        using type = fixedpoint<larger_base_type, larger_operational_type, max(a_fixed::fraction_bits, b_fixed::fraction_bits)>;
    };

        
    template<typename BASE, typename ARG>
    struct result_type_base {
        constexpr const static bool is_signed = std::is_signed<BASE>::value || std::is_signed<ARG>::value;
        using base_fixed = as_fixed<BASE>;
        using arg_fixed = as_fixed<ARG>;
        using larger_operational_type = typename set_sign<is_signed, typename std::conditional<(sizeof(typename base_fixed::CALCULATE_TYPE) > sizeof(typename arg_fixed::CALCULATE_TYPE)), typename base_fixed::CALCULATE_TYPE, typename arg_fixed::CALCULATE_TYPE>::type>::type;
        using type = fixedpoint<typename base_fixed::BUF_TYPE, larger_operational_type, base_fixed::fraction_bits>;
    };


    template<typename A, typename B, typename C = typename result_type<A, B>::type, typename std::enable_if<is_fixedpoint<C>::value, void*>::type = nullptr>
    struct fixed_operations {

        template<typename U> constexpr static typename C::BUF_TYPE make_c_buf(const U x) {
            return make_buf<U, typename C::BUF_TYPE, C::fraction_bits>(x);
        }

        constexpr static C add(const A a, const B b) noexcept {
            return C::buf_cast(make_c_buf<A>(a) + make_c_buf<B>(b));
        }
                
        constexpr static C sub(const A a, const B b) noexcept {
            return C::buf_cast(make_c_buf<A>(a) - make_c_buf<B>(b));
        }

        #define FIXED_OPERATIONS_OPERATOR_MAKER(name, operator)            \
        constexpr static bool name(const A a, const B b) noexcept {        \
            return make_c_buf<A>(a) operator make_c_buf<B>(b);   \
        }

        FIXED_OPERATIONS_OPERATOR_MAKER(eq, ==)
        FIXED_OPERATIONS_OPERATOR_MAKER(neq, !=)
        FIXED_OPERATIONS_OPERATOR_MAKER(gtr, >)
        FIXED_OPERATIONS_OPERATOR_MAKER(geq, >=)
        FIXED_OPERATIONS_OPERATOR_MAKER(lss, <)
        FIXED_OPERATIONS_OPERATOR_MAKER(leq, <=)

        #undef FIXED_OPERATIONS_OPERATOR_MAKER

        constexpr const static int a_acc = static_accuracy<A, C::fraction_bits>::value;
        constexpr const static int b_acc = static_accuracy<B, C::fraction_bits>::value;
        constexpr const static int c_acc = C::fraction_bits;

        constexpr const static int mul_raw_accuracy = a_acc + b_acc;
        constexpr const static int mul_accuracy_decrease = mul_raw_accuracy - c_acc;        
        constexpr const static int mul_max_result_decrease = (sizeof(typename C::CALCULATE_TYPE) - sizeof(typename C::BUF_TYPE)) * 8;
        constexpr const static int mul_c_decrease = min(mul_max_result_decrease, mul_accuracy_decrease);
        constexpr const static int mul_ab_decrease = mul_accuracy_decrease - mul_c_decrease;
        constexpr const static int mul_a_decrease = mul_ab_decrease * a_acc / (a_acc + b_acc);
        constexpr const static int mul_b_decrease = mul_ab_decrease - mul_a_decrease;

        constexpr static C multiple(const A a, const B b) noexcept {
            return C::buf_cast(static_signed_shl<typename C::CALCULATE_TYPE, -mul_c_decrease>(make_buf<A, typename C::CALCULATE_TYPE, a_acc - mul_a_decrease>(a) * make_buf<B, typename C::CALCULATE_TYPE, b_acc - mul_b_decrease>(b)));
        }


        constexpr const static int div_raw_accuracy = a_acc - b_acc;
        constexpr const static int div_accuracy_increase = c_acc - div_raw_accuracy;
        constexpr const static int div_max_a_increase = (sizeof(typename C::CALCULATE_TYPE) - sizeof(typename as_fixed<A>::BUF_TYPE)) * 8;
        constexpr const static int div_a_increase = min(max(div_max_a_increase, c_acc - a_acc), div_accuracy_increase);
        constexpr const static int div_bc_change = div_accuracy_increase - div_a_increase;
        constexpr const static int div_b_decrease = div_bc_change * b_acc / (b_acc + c_acc);
        constexpr const static int div_c_increase = div_bc_change - div_b_decrease;

        constexpr static C divide(const A a, const B b) noexcept {
            typename C::BUF_TYPE divisor = make_buf<B, typename C::BUF_TYPE, b_acc - div_b_decrease>(b);            
            return (divisor) ?
                C::buf_cast(static_signed_shl<typename C::CALCULATE_TYPE, div_c_increase>(make_buf<A, typename C::CALCULATE_TYPE, a_acc + div_a_increase>(a) / divisor)) :
                ((a < 0) ? std::numeric_limits<C>::lowest() : std::numeric_limits<C>::max());
        }


        constexpr static C modulo(const A a, const B b) noexcept {
            return C::buf_cast(make_buf<A, typename C::BUF_TYPE, c_acc>(a) % make_buf<B, typename C::BUF_TYPE, c_acc>(b));
        }

    };


}


#define FIXED_POINT_BOOL_TEMPLATE template<typename B, typename std::enable_if<std::is_same<B, bool>::value, B>::type* = nullptr>
#define FIXED_POINT_INTEGER_TEMPLATE template<typename I, typename std::enable_if<std::is_integral<I>::value && !std::is_same<I, bool>::value, I>::type* = nullptr>
#define FIXED_POINT_FLOAT_TEMPLATE template<typename FP, typename std::enable_if<std::is_floating_point<FP>::value, FP>::type* = nullptr>        


template<typename T, typename TC=typename fixedpoint_helpers::make_fast_int<T>::type, unsigned frac_bits=sizeof(T)*4-1>
class fixedpoint {

    public:

        using BUF_TYPE = T;
        using CALCULATE_TYPE = TC;
        constexpr static const unsigned fraction_bits = frac_bits;

        template<typename N, typename std::enable_if<std::is_floating_point<N>::value || fixedpoint_helpers::is_fixedpoint<N>::value || (std::is_integral<N>::value && !std::is_same<bool, N>::value), N>::type* = nullptr>
        constexpr fixedpoint(const N value) noexcept : buf(fixedpoint_helpers::make_buf<N, T, frac_bits>(value)) {}

        constexpr fixedpoint() noexcept : buf(0) {}

        constexpr fixedpoint(const fixedpoint& another) noexcept = default;

        constexpr fixedpoint(fixedpoint&& another) noexcept = default;

        ~fixedpoint() noexcept = default;

        template<typename T2, typename TC2>
        constexpr fixedpoint(const fixedpoint<T2, TC2, frac_bits>& another) noexcept
            : buf(another.buf) {}

        template<typename T2, typename TC2, unsigned frac_bits2>
        constexpr fixedpoint(const fixedpoint<T2, TC2, frac_bits2>& another) noexcept
            : buf((frac_bits > frac_bits2) ? ((T)another.buf << (frac_bits - frac_bits2)) : (another.buf >> (frac_bits2 - frac_bits))) {}

        static fixedpoint from_float(float x) {
            return fixedpoint(fixedpoint_helpers::buf_from_ieee754<BUF_TYPE, frac_bits>(x), true);
        }

        static fixedpoint from_double(double x) {
            return fixedpoint(fixedpoint_helpers::buf_from_ieee754<BUF_TYPE, frac_bits>(x), true);
        }

        FIXED_POINT_FLOAT_TEMPLATE
        static fixedpoint from_float_stable(FP x) {
            return fixedpoint(x * (1 << frac_bits), true);
        }
        

        constexpr static fixedpoint buf_cast(const T buf) noexcept {
            return fixedpoint(buf, true);
        }

        constexpr static fixedpoint fraction(const T& counter, const T& denimonator) noexcept {
            return  fixedpoint((counter << frac_bits) / denimonator, true);
        }

        FIXED_POINT_INTEGER_TEMPLATE
        constexpr friend fixedpoint operator<<(const fixedpoint first, const I second) noexcept {
            return fixedpoint(first.buf << second, true);
        }

        FIXED_POINT_INTEGER_TEMPLATE
        constexpr friend fixedpoint operator>>(const fixedpoint first, const I second) noexcept {
            return fixedpoint(first.buf >> second, true);
        }


        fixedpoint& operator=(const fixedpoint&) = default;

        fixedpoint& operator=(fixedpoint&& other) = default;


        FIXED_POINT_INTEGER_TEMPLATE
        void operator<<=(const I another) noexcept {
            buf <<= another;
        }

        FIXED_POINT_INTEGER_TEMPLATE
        void operator>>=(const I another) noexcept {
            buf >>= another;
        }

        constexpr fixedpoint operator+() const noexcept {
            return fixedpoint(buf, true);
        }

        constexpr fixedpoint operator-() const noexcept {
            return fixedpoint(-buf, true);
        }

        FIXED_POINT_FLOAT_TEMPLATE
        constexpr operator FP() const noexcept {
            constexpr const FP k = ((FP)1) / ((FP)((T)1 << frac_bits));
            return (FP)buf * k;
        }

        FIXED_POINT_INTEGER_TEMPLATE
        constexpr operator I() const noexcept {
            return buf >> frac_bits;
        }

        FIXED_POINT_BOOL_TEMPLATE
        constexpr operator B() noexcept {
            return (B)buf;
        }

        constexpr T getBuf() const noexcept {
            return buf;
        }

        constexpr bool isNeg() const noexcept {
            return buf < 0;
        }

        constexpr bool isPos() const noexcept {
            return buf > 0;
        }

        constexpr bool isZero() const noexcept {
            return buf == 0;
        }

        constexpr bool isNotZero() const noexcept {
            return buf != 0;
        }

        constexpr T getfrac() const noexcept {
            return buf & (((T)1 << frac_bits) - 1);
        }

        unsigned toCharBuf(char* buffer, unsigned char base=10, unsigned max_frac_digits=(unsigned)(-1)) const {
            char* wbuf = buffer;
            T tmpBuf = buf;
            if (tmpBuf < 0) {
                *(wbuf++) = '-';
                tmpBuf = -tmpBuf;
                if (tmpBuf < 0) {
                    *(wbuf++) = '-';
                    *(wbuf++) = 'i';
                    *(wbuf++) = 'n';
                    *(wbuf++) = 'f';
                    *wbuf = 0;
                    return wbuf - buffer;
                }
            }
            T intPart = tmpBuf >> frac_bits;
            T fracPart = tmpBuf - (intPart << frac_bits);
            char* swapStart = wbuf - 1;
            do {
                *(wbuf++) = num2chr(intPart % base);
                intPart /= base;
            } while (intPart > 0);
            char* swapEnd = wbuf;
            while (++swapStart < --swapEnd) {
                *swapStart ^= *swapEnd;
                *swapEnd ^= *swapStart;
                *swapStart ^= *swapEnd;
            }
            if (fracPart) {
                *(wbuf++) = '.';
                while (fracPart && max_frac_digits--) {
                    fracPart *= 10;
                    T digit = fracPart >> frac_bits;
                    fracPart -= digit << frac_bits;
                    *(wbuf++) = num2chr(digit);
                }
            }
            *wbuf = 0;
            return wbuf - buffer;
        }

        static fixedpoint fromCharBuf(const char* buffer, unsigned char base=10, unsigned* size=NULL) {
            fixedpoint result = 0;
            fixedpoint multiplier = 1;
            bool frac_mode = false;
            unsigned position = 0;
            unsigned max_size = size ? *size : (unsigned)(-1);
            bool minus = buffer[position] == '-';
            if (buffer[position] == '-' || buffer[position] == '+') position++;
            position--;
            while (++position < max_size && buffer[position] && multiplier) {
                if (buffer[position] == '.') {
                    if (frac_mode) break;
                    frac_mode = true;
                    continue;
                }
                unsigned num = chr2num(buffer[position]);
                if (num >= base) break;
                if (frac_mode) {
                    multiplier /= base;
                    result += multiplier * num;
                } else {
                    result *= base;
                    result += num;
                }
            }
            if (size) *size = position;
            return minus ? -result : result;
        }

        // each bit (the lowest base) + sign + point + zero-char + padding
        using stream_buf_size = std::integral_constant<unsigned, sizeof(T)*8+4>;

        template<class string_type>
        string_type toString(unsigned char base=10, unsigned max_frac_digits=(unsigned)(-1)) const {
            char buf[stream_buf_size::value];
            toCharBuf(buf, base, max_frac_digits);
            return string_type((const char*)buf);
        }

        template<class string_type>
        static fixedpoint fromString(const string_type& str, unsigned char base=10, unsigned* size=NULL) {
            return fixedpoint::fromCharBuf(str.c_str(), base, size);
        }

        template<class Stream>
        friend Stream& operator << (Stream& stream, const fixedpoint fp) {
            char buf[stream_buf_size::value];
            fp.toCharBuf(buf);
            stream << buf;
            return stream;
        }

        template<class Stream>
        friend Stream& operator >> (Stream& stream, fixedpoint& fp) {
            char buf[stream_buf_size::value];
            unsigned position = 0;
            char peeked = stream.peek();
            if (peeked == '-' || peeked == '+') buf[position++] = stream.get();
            while (1) {
                peeked = stream.peek();
                if (peeked >= '0' && peeked <= '9')
                    buf[position++] = stream.get();
                else break;
            }
            if (stream.peek() == '.') {
                buf[position++] = stream.get();
                while (1) {
                    peeked = stream.peek();
                    if (peeked >= '0' && peeked <= '9')
                        buf[position++] = stream.get();
                    else break;
                }
            }
            buf[position++] = 0;
            fp = fixedpoint::fromCharBuf(buf);
            return stream;
        }

        friend struct std::numeric_limits<fixedpoint<T, TC, frac_bits>>;

        template<typename T2, typename TC2, unsigned frac_bits2>
        friend class fixedpoint;

        static_assert(std::is_arithmetic<T>::value, "Type for the buf must be arithmetic.");
        static_assert(std::is_arithmetic<TC>::value, "Type helper for multiplication must be arithmetic.");
        static_assert(std::numeric_limits<T>::radix == 2, "Type for the buf must be binary (radix == 2).");
        static_assert(std::numeric_limits<TC>::radix == 2, "Type helper for multiplication must be binary (radix == 2).");
        static_assert(std::numeric_limits<T>::is_integer, "Type for the buf must be an integer type.");
        static_assert(std::numeric_limits<TC>::is_integer, "Type for multiplication must be an integer type.");
        static_assert(std::is_signed<T>::value == std::is_signed<TC>::value, "T and TC must be both signed or unsigned.");
        static_assert(sizeof(T) * 8 - std::is_signed<T>::value > frac_bits, "There must be less fraction bits than number of bits in the buf type.");
        static_assert(sizeof(T) <= sizeof(TC), "Type for multiplication must be equal or greater than buf type.");

        #undef FIXED_POINT_BOOL_TEMPLATE
        #undef FIXED_POINT_INTEGER_TEMPLATE
        #undef FIXED_POINT_FLOAT_TEMPLATE

    private:
        T buf;

        constexpr fixedpoint(const T new_buf, bool) noexcept : buf(new_buf) {}

        static constexpr unsigned char num2chr(unsigned char num) noexcept {
            return (num <= 9) ? ('0' + num) : (('a' - 10) + num);
        }

        static constexpr unsigned char chr2num(unsigned char chr) noexcept {
            return (chr <= '9') ? (chr - '0') : (chr >= 'a') ? (chr - ('a' - 10)) : (chr - ('A' - 10));
        }

};


#define FIXED_POINT_AT_LEAST_ONE template<typename A, typename B, typename std::enable_if<fixedpoint_helpers::is_fixedpoint<A>::value || fixedpoint_helpers::is_fixedpoint<B>::value, void*>::type = nullptr>

FIXED_POINT_AT_LEAST_ONE
constexpr auto operator+(const A a, const B b) noexcept {
    return fixedpoint_helpers::fixed_operations<A, B>::add(a, b);
}

FIXED_POINT_AT_LEAST_ONE
constexpr auto operator-(const A a, const B b) noexcept {
    return fixedpoint_helpers::fixed_operations<A, B>::sub(a, b);
}

FIXED_POINT_AT_LEAST_ONE
constexpr auto operator*(const A a, const B b) noexcept {
    return fixedpoint_helpers::fixed_operations<A, B>::multiple(a, b);
}

FIXED_POINT_AT_LEAST_ONE
constexpr auto operator/(const A a, const B b) noexcept {
    return fixedpoint_helpers::fixed_operations<A, B>::divide(a, b);
}

FIXED_POINT_AT_LEAST_ONE
constexpr auto operator%(const A a, const B b) noexcept {
    return fixedpoint_helpers::fixed_operations<A, B>::modulo(a, b);
}


#define FIXED_POINT_OPERATOR_MAKER(oper, name)                      \
FIXED_POINT_AT_LEAST_ONE                                            \
constexpr bool operator oper(const A a, const B b) noexcept {       \
    return fixedpoint_helpers::fixed_operations<A, B>::name(a, b);  \
}

FIXED_POINT_OPERATOR_MAKER(==, eq)
FIXED_POINT_OPERATOR_MAKER(!=, neq)
FIXED_POINT_OPERATOR_MAKER(>=, geq)
FIXED_POINT_OPERATOR_MAKER(<=, leq)
FIXED_POINT_OPERATOR_MAKER(>, gtr)
FIXED_POINT_OPERATOR_MAKER(<, lss)
#undef FIXED_POINT_OPERATOR_MAKER

#define FIXED_POINT_FIRST  template<typename A, typename B, typename std::enable_if<fixedpoint_helpers::is_fixedpoint<A>::value, void*>::type = nullptr>

FIXED_POINT_FIRST
constexpr void operator+=(A& a, const B b) noexcept {
    a = fixedpoint_helpers::fixed_operations<A, B, typename fixedpoint_helpers::result_type_base<A, B>::type>::add(a, b);
}

FIXED_POINT_FIRST
constexpr void operator-=(A& a, const B b) noexcept {
    a = fixedpoint_helpers::fixed_operations<A, B, typename fixedpoint_helpers::result_type_base<A, B>::type>::sub(a, b);
}

FIXED_POINT_FIRST
constexpr void operator*=(A& a, const B b) noexcept {
    a = fixedpoint_helpers::fixed_operations<A, B, typename fixedpoint_helpers::result_type_base<A, B>::type>::multiple(a, b);
}

FIXED_POINT_FIRST
constexpr void operator/=(A& a, const B b) noexcept {
    a = fixedpoint_helpers::fixed_operations<A, B, typename fixedpoint_helpers::result_type_base<A, B>::type>::divide(a, b);
}

FIXED_POINT_FIRST
constexpr void operator%=(A& a, const B b) noexcept {
    a = fixedpoint_helpers::fixed_operations<A, B, typename fixedpoint_helpers::result_type_base<A, B>::type>::modulo(a, b);
}

#undef FIXED_POINT_FIRST


namespace std {

    template<typename T, typename TC, unsigned frac_bits>
    constexpr T floor(const fixedpoint<T, TC, frac_bits> x) noexcept {
        return x.getBuf() >> frac_bits;
    }

    template<typename T, typename TC, unsigned frac_bits>
    constexpr T ceil(const fixedpoint<T, TC, frac_bits> x) noexcept {
        return (x.getBuf() >> frac_bits) + !!x.getfrac();
    }

    template<typename T, typename TC, unsigned frac_bits>
    constexpr T round(const fixedpoint<T, TC, frac_bits> x) noexcept {
        return (x.getBuf() >> frac_bits) + (x.getfrac() >> (frac_bits-1));
    }

    template<typename T, typename TC, unsigned frac_bits>
    constexpr fixedpoint<T, TC, frac_bits> abs(const fixedpoint<T, TC, frac_bits> x) noexcept {
        return x.isNeg() ? -x : x;
    }

    template<typename T, typename TC, unsigned frac_bits>
    constexpr fixedpoint<T, TC, frac_bits> sign(const fixedpoint<T, TC, frac_bits> x) noexcept {
        return x.getBuf() ? (x.isNeg() ? -1 : 1) : 0;
    }

    template<typename T, typename TC, unsigned frac_bits>
    struct hash<fixedpoint<T, TC, frac_bits>> {

        std::size_t operator()(const fixedpoint<T, TC, frac_bits>& fp) const noexcept {
            std::size_t buf = hash_t(fp.getBuf()) ^ ((frac_bits - 1) * (sizeof(T)-1));
            return buf + (buf << frac_bits) + (buf >> frac_bits);
        }

        std::hash<T> hash_t;
    };

    template<typename T, typename TC, unsigned frac_bits>
    struct is_arithmetic<fixedpoint<T, TC, frac_bits>> : public std::true_type {};

    template<typename T, typename TC, unsigned frac_bits>
    struct is_scalar<fixedpoint<T, TC, frac_bits>> : public std::true_type {};

    template<typename T, typename TC, unsigned frac_bits>
    struct is_object<fixedpoint<T, TC, frac_bits>> : public std::true_type {};

    template<typename T, typename TC, unsigned frac_bits>
    struct is_signed<fixedpoint<T, TC, frac_bits>> : public std::is_signed<T> {};

    template<typename T, typename TC, unsigned frac_bits>
    struct is_unsigned<fixedpoint<T, TC, frac_bits>> : public std::is_unsigned<T> {};

    template<typename T, typename TC, unsigned frac_bits>
    struct make_signed<fixedpoint<T, TC, frac_bits>> {
        using type = fixedpoint<typename make_signed<T>::type, typename make_signed<TC>::type, frac_bits>;
    };

    template<typename T, typename TC, unsigned frac_bits>
    struct make_unsigned<fixedpoint<T, TC, frac_bits>> {
        using type = fixedpoint<typename make_unsigned<T>::type, typename make_unsigned<TC>::type, frac_bits>;
    };

    template<typename T, typename TC, unsigned frac_bits>
    struct numeric_limits<fixedpoint<T, TC, frac_bits>> {

        using numeric_limits_t = numeric_limits<T>;
        using fp = fixedpoint<T, TC, frac_bits>;

        static constexpr const auto is_specialized = true;
        static constexpr const auto is_signed = (bool)std::is_signed<T>::value;
        static constexpr const auto is_integer = false;
        static constexpr const auto is_exact = numeric_limits_t::is_exact;
        static constexpr const auto has_infinity = numeric_limits_t::has_infinity;
        static constexpr const auto has_quiet_NaN = numeric_limits_t::has_quiet_NaN;
        static constexpr const auto has_denorm = std::denorm_absent;
        static constexpr const auto has_denorm_loss = false;
        static constexpr const auto round_style = numeric_limits_t::round_style;
        static constexpr const auto is_iec559 = false;
        static constexpr const auto digits = numeric_limits_t::digits;
        static constexpr const auto digits10 = numeric_limits_t::digits10;
        static constexpr const auto radix = 2;
        static constexpr const auto min_exponent = 0;
        static constexpr const auto min_exponent10 = 0;
        static constexpr const auto max_exponent = 0;
        static constexpr const auto max_exponent10 = 0;
        static constexpr const auto traps = numeric_limits_t::traps;
        static constexpr const auto tinyness_before = numeric_limits_t::tinyness_before;

        static constexpr fp min() noexcept {
            return fp(1, true);
        }

        static constexpr fp lowest() noexcept {
            return fp(numeric_limits_t::lowest(), true);
        }

        static constexpr fp max() noexcept {
            return fp(numeric_limits_t::max(), true);
        }

        static constexpr fp epsilon() noexcept {
            return fp(1, true);
        }

        static constexpr fp infinity() noexcept {
            return fp(numeric_limits_t::infinity(), true);
        }

        static constexpr fp quiet_NaN() noexcept {
            return fp(numeric_limits_t::quiet_NaN(), true);
        }

        static constexpr fp signaling_NaN() noexcept {
            return fp(numeric_limits_t::signaling_NaN(), true);
        }

        static constexpr fp denorm_min() noexcept {
            return fp(1, true);
        }

        using float_round_style = std::float_round_style;
        using float_denorm_style = std::float_denorm_style;

    };

}

// standard types
using fixed8 = fixedpoint<std::int8_t>;
using fixed16 = fixedpoint<std::int16_t>;
using fixed32 = fixedpoint<std::int32_t>;
using fixed64 = fixedpoint<std::int64_t>;

using ufixed8 = fixedpoint<std::uint8_t>;
using ufixed16 = fixedpoint<std::uint16_t>;
using ufixed32 = fixedpoint<std::uint32_t>;
using ufixed64 = fixedpoint<std::uint64_t>;

// accurate types - multiplication with larger int - if it is possible
using fixed8_a = fixedpoint<std::int8_t, fixedpoint_helpers::make_fast_int<std::int16_t>::type>;
using fixed16_a = fixedpoint<std::int16_t, fixedpoint_helpers::make_fast_int<std::int32_t>::type>;
using fixed32_a = fixedpoint<std::int32_t, fixedpoint_helpers::make_fast_int<std::int64_t>::type>;
using fixed64_a = fixed64;

using ufixed8_a = fixedpoint<std::uint8_t, fixedpoint_helpers::make_fast_int<std::uint16_t>::type>;
using ufixed16_a = fixedpoint<std::uint16_t, fixedpoint_helpers::make_fast_int<std::uint32_t>::type>;
using ufixed32_a = fixedpoint<std::uint32_t, fixedpoint_helpers::make_fast_int<std::uint64_t>::type>;
using ufixed64_a = ufixed64;

// simple types - multiplication with the same size of int
using fixed8_s = fixedpoint<std::int8_t, std::int8_t>;
using fixed16_s = fixedpoint<std::int16_t, std::int16_t>;
using fixed32_s = fixedpoint<std::int32_t, std::int32_t>;
using fixed64_s = fixedpoint<std::int64_t, std::int64_t>;

using ufixed8_s = fixedpoint<std::uint8_t, std::uint8_t>;
using ufixed16_s = fixedpoint<std::uint16_t, std::uint16_t>;
using ufixed32_s = fixedpoint<std::uint32_t, std::uint32_t>;
using ufixed64_s = fixedpoint<std::uint64_t, std::uint64_t>;


#endif
