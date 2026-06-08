#pragma once
#include <dme/math/utils.h>
namespace dme {
    namespace math {
        //弧度，值范围-pi<=v<pi
        struct Radian {
            //不要直接修改该值
            float v;

            constexpr Radian() noexcept : v(0) {}
            //不要传入大于2pi小于-2pi的数
            constexpr Radian(float v) noexcept : v(_fix_radian_(v)) {}
            constexpr Radian(const Radian& a) noexcept : v(_fix_radian_(a.v)) {}
            constexpr Radian(Radian&& a) noexcept : v(_fix_radian_(a.v)) {}

            //不要传入大于2pi小于-2pi的数
            constexpr static inline float _fix_radian_(float a) noexcept {
                if (a < -math::pi) {
                    return a + math::pi2;
                }
                else if (a >= math::pi) {
                    return a - math::pi2;
                }
                return a;
            }

            inline float sin() const noexcept;
            inline float cos() const noexcept;
            inline float tan() const noexcept;

            static inline float Sin(const Radian& a) noexcept;
            static inline float Cos(const Radian& a) noexcept;
            static inline float Tan(const Radian& a) noexcept;

            Radian& operator =(float a) noexcept;
            Radian& operator =(const Radian& a) noexcept;
            Radian& operator =(Radian&& a) noexcept;

            Radian operator +(float a) const noexcept;
            Radian operator +(const Radian& a) const noexcept;
            void operator +=(float a) noexcept;
            void operator +=(const Radian& a) noexcept;

            Radian operator -(float a) const noexcept;
            Radian operator -(const Radian& a) const noexcept;
            void operator -=(float a) noexcept;
            void operator -=(const Radian& a) noexcept;

            bool operator ==(const Radian& a) const noexcept;
            size_t operator ()(const Radian& a) const noexcept;
            friend std::ostream& operator<<(std::ostream& os, const Radian& a);

        };
    }
}