#include <dme/math/radian.h>
#include <dme/math/utils.h>
#include <dme/core/hash.h>
namespace dme::math {

    inline float Radian::sin() const noexcept {
        return std::sin(v);
    }
    inline float Radian::cos() const noexcept {
        return std::cos(v);
    }
    inline float Radian::tan() const noexcept {
        return std::tan(v);
    }

    inline float Radian::Sin(const Radian& a) noexcept {
        return std::sin(a.v);
    }
    inline float Radian::Cos(const Radian& a) noexcept {
        return std::cos(a.v);
    }
    inline float Radian::Tan(const Radian& a) noexcept {
        return std::tan(a.v);
    }

    Radian& Radian::operator =(float a) noexcept {
        v = a;
        return *this;
    }
    Radian& Radian::operator =(const Radian& a) noexcept {
        v = a.v;
        return *this;
    }
    Radian& Radian::operator =(Radian&& a) noexcept {
        v = a.v;
        return *this;
    }

    Radian Radian::operator+(float a) const noexcept {
        return Radian(_fix_radian_(v + a));
    }
    Radian Radian::operator+(const Radian& a) const noexcept {
        return Radian(_fix_radian_(v + a.v));
    }
    void Radian::operator+=(float a) noexcept {
        v = _fix_radian_(v + a);
    }
    void Radian::operator+=(const Radian& a) noexcept {
        v = _fix_radian_(v + a.v);
    }

    Radian Radian::operator-(float a) const noexcept {
        return Radian(_fix_radian_(v - a));
    }
    Radian Radian::operator-(const Radian& a) const noexcept {
        return Radian(_fix_radian_(v - a.v));
    }
    void Radian::operator-=(float a) noexcept {
        v = _fix_radian_(v - a);
    }
    void Radian::operator-=(const Radian& a) noexcept {
        v = _fix_radian_(v - a.v);
    }

    bool Radian::operator ==(const Radian& a) const noexcept {
        return v == a.v;
    }
    size_t Radian::operator ()(const Radian& a) const noexcept {
        std::hash<float> hasher;
        return hasher(a.v);
    }
    std::ostream& math::operator<<(std::ostream& os, const Radian& a) {
        return os << "value: " << a.v;
    }

}
