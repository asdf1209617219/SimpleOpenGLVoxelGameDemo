#pragma once
#include <dme/core/utils.h>
namespace dme {
    namespace math {
        //π
        constexpr float pi = static_cast<float>(3.14159265358979323846264338327950288);
        //π * 2
        constexpr float pi2 = static_cast<float>(3.14159265358979323846264338327950288 * 2.0);
        //π / 2
        constexpr float piover2 = static_cast<float>(3.14159265358979323846264338327950288 / 2.0);
        //√2
        constexpr float sqrt2 = static_cast<float>(1.4142135623730950488016887242097);
        //1/√2
        constexpr float sqrt2_inv = static_cast<float>(1 / 1.4142135623730950488016887242097);
        //√3
        constexpr float sqrt3 = static_cast<float>(1.7320508075688772935274463415059);
        //1/√3
        constexpr float sqrt3_inv = static_cast<float>(1 / 1.7320508075688772935274463415059);


        using axis_t = uchar;
        using axis_st = schar;
        struct Axis2 {
            axis_t v;

            constexpr Axis2() noexcept : v(0) {};
            constexpr Axis2(axis_t v) noexcept : v(v) {}

            inline constexpr operator axis_t() const noexcept {
                return v;
            }
        };
        struct Axis2c {
            static constexpr Axis2 x = Axis2(0);
            static constexpr Axis2 y = Axis2(1);
        };

        struct Axis3 {
            axis_t v;

            constexpr Axis3() noexcept : v(0) {};
            constexpr Axis3(axis_t v) noexcept : v(v) {}

            inline constexpr operator axis_t() const noexcept {
                return v;
            }
        };
        struct Axis3c {
            static constexpr Axis3 x = Axis3(0);
            static constexpr Axis3 y = Axis3(1);
            static constexpr Axis3 z = Axis3(2);
        };

        struct Axis4 {
            axis_t v;

            constexpr Axis4() noexcept : v(0) {};
            constexpr Axis4(axis_t v) noexcept : v(v) {}

            inline constexpr operator axis_t() const noexcept {
                return v;
            }
        };
        struct Axis4c {
            static constexpr Axis4 x = Axis4(0);
            static constexpr Axis4 y = Axis4(1);
            static constexpr Axis4 z = Axis4(2);
            static constexpr Axis4 w = Axis4(3);
        };

        // 返回a/b，如果b为0则会返回0；
        // 返回类型规则：
        // a和b中有浮点数的话返回类型就是浮点数，而且会优先使用精度更高的浮点数（如果a或b是的话）
        // a和b中如果都是整型，两个都是有符号整数的情况下返回有符号的，否则返回无符号的
        template<IsBaseNumber T1, IsBaseNumber T2>
        inline constexpr auto divide(T1 a, T2 b) noexcept {
            return arithmetic_return_type<T1, T2>(b == 0 ? 0 : a / b);
        }

        // 返回a%b，如果b为0则会返回0；
        template<IsBaseNumber T1, IsBaseNumber T2>
        inline constexpr auto modulo(T1 a, T2 b) noexcept {
            return arithmetic_return_type<T1, T2>(b == 0 ? 0 : a % b);
        }

        //返回浮点数向下取整的整数版本
        inline constexpr sint floori(float n) noexcept {
            sint i = static_cast<sint>(n);
            return n < i ? (i - 1) : i;
        }
        //返回浮点数向下取整的整数版本
        inline constexpr sint floori(double n) noexcept {
            sint i = static_cast<sint>(n);
            return n < i ? (i - 1) : i;
        }
        //返回浮点数向上取整的整数版本
        inline constexpr sint ceili(float n) noexcept {
            sint i = static_cast<sint>(n);
            return n > i ? (i + 1) : i;
        }
        //返回浮点数向上取整的整数版本
        inline constexpr sint ceili(double n) noexcept {
            sint i = static_cast<sint>(n);
            return n > i ? (i + 1) : i;
        }

        //角度制转弧度制乘以的系数
        constexpr float radians_factor = static_cast<float>(0.01745329251994329576923690768489);
        //弧度制转角度制乘以的系数
        constexpr float degrees_factor = static_cast<float>(57.295779513082320876798154814105);
        //角度制转换为弧度制
        inline constexpr float radians(float degrees) noexcept {
            return degrees * radians_factor;
        }
        //弧度制转换为角度制
        inline constexpr float degrees(float radians) noexcept {
            return radians * degrees_factor;
        }
        //角度转动超过一圈，自动加上2pi或-2pi，取值范围[-pi, pi)
        inline constexpr float circle(float radians) noexcept {
            if (radians < -math::pi) {
                return radians + math::pi2;
            }
            else if (radians >= math::pi) {
                return radians - math::pi2;
            }
            else {
                return radians;
            }
        }

        //true返回1，false返回-1
        inline constexpr schar positiveOne(bool isPositive) noexcept {
            //return (isPositive << 1) - 1;
            return isPositive ? 1 : -1;
        }
        //true返回-1，false返回1
        inline constexpr schar negativeOne(bool isNegative) noexcept {
            //return 1 - (isNegative << 1);
            return isNegative ? -1 : 1;
        }
        //带方向的比较大小，正向等价于小于，负向等价于大于
        inline constexpr bool compareDirection(sint a, sint b, sint directionSign) noexcept {
            return directionSign > 0 ? a < b : a > b;
        }

        template <IsUnsigned T>
        //对无符号类型取负数（即取补码，依然返回无符号类型）
        DME_INLINE constexpr T negativeUnsiged(const T& a) noexcept {
            return static_cast<T>(-static_cast<std::make_signed_t<T>>(a));
        }

        template <IsUnsigned T1, IsUnsigned T2>
        //无符号类型的绝对差
        DME_INLINE constexpr auto Udiff(const T1& a, const T2& b) noexcept {
            return a < b ? (b - a) : (a - b);
        }

        template <typename T1, typename T2>
        //两值的绝对差，会根据数学上的概念正确处理无符号类型的情况，除非两个值的差值超过了无符号类型的范围
        DME_INLINE constexpr auto Diff(const T1& a, const T2& b) noexcept {
            if constexpr (is_unsigned<T1> && !is_unsigned<T2>) {
                return b < 0 ? (a + get_bigger_type<T1, T2>(-b)) : (a < b ? (b - a) : (a - b));
            }
            if constexpr (!is_unsigned<T1> && is_unsigned<T2>) {
                return a < 0 ? (b + get_bigger_type<T2, T1>(-a)) : (a < b ? (b - a) : (a - b));
            }
            else {
                return a < b ? (b - a) : (a - b);
            }
        }

        template <typename T1, typename T2>
        //返回类型为自动
        DME_INLINE constexpr auto Min(const T1& a, const T2& b) noexcept {
            return a < b ? a : b;
        }

        //手动指定返回类型
        template <typename R, typename T1, typename T2>
        DME_INLINE constexpr R MinC(const T1& a, const T2& b) noexcept {
            return a < b ? R(a) : R(b);
        }

        template <typename T1, typename T2>
        //返回类型为自动
        DME_INLINE constexpr auto Max(const T1& a, const T2& b) noexcept {
            return a > b ? a : b;
        }

        //手动指定返回类型
        template <typename R, typename T1, typename T2>
        DME_INLINE constexpr R MaxC(const T1& a, const T2& b) noexcept {
            return a > b ? R(a) : R(b);
        }

        template <typename T>
        DME_INLINE constexpr auto Abs(const T& a) noexcept {
            if constexpr (is_safe_negative_sign<T>) {
                return a > T(0) ? a : -a;
            }
            else {
                return a;
            }
        }

        template <typename... Args>
        //返回类型为自动
        DME_INLINE constexpr auto Avg(const Args&... args) noexcept {
            return (args + ...) / sizeof...(Args);
        }
        template <typename R, typename... Args>
        //手动指定返回类型
        DME_INLINE constexpr auto AvgC(const Args&... args) noexcept {
            return R(R((args + ...)) / R(sizeof...(Args)));
        }

        //返回类型为第一个参数的类型
        template <typename R, typename T1, typename T2>
        DME_INLINE constexpr R Clamp(const R& a, const T1& low, const T2& high) noexcept {
            return MaxC<R, T1, R>(low, MinC<R, R, T2>(a, high));
        }
    }
    using math::Axis2; using math::Axis3; using math::Axis4;
    using math::Axis2c; using math::Axis3c; using math::Axis4c;
}

namespace std {
    template<>
    struct hash<dme::math::Axis2> {
        inline size_t operator ()(const dme::math::Axis2& a) const noexcept {
            return dme::Hasher(a);
        }
    };
    template<>
    struct hash<dme::math::Axis3> {
        inline size_t operator ()(const dme::math::Axis3& a) const noexcept {
            return dme::Hasher(a);
        }
    };
    template<>
    struct hash<dme::math::Axis4> {
        inline size_t operator ()(const dme::math::Axis4& a) const noexcept {
            return dme::Hasher(a);
        }
    };
}