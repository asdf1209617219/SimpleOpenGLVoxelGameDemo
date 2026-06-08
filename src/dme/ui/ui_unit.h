#pragma once
#include <dme/core.h>
namespace dme::ui {
	//像素
	class UIpx {
		float v;
	public:
		constexpr explicit UIpx(float v = 0) noexcept : v(v) {}
		constexpr float val() const noexcept {
			return v;
		}

		constexpr UIpx operator+(float a) const noexcept {
			return UIpx(v + a);
		}
		constexpr UIpx& operator+=(float a) noexcept {
			v += a;
			return *this;
		}
		constexpr UIpx operator-(float a) const noexcept {
			return UIpx(v - a);
		}
		constexpr UIpx& operator-=(float a) noexcept {
			v -= a;
			return *this;
		}
		constexpr UIpx operator*(float a) const noexcept {
			return UIpx(v * a);
		}
		constexpr UIpx& operator*=(float a) noexcept {
			v *= a;
			return *this;
		}
		constexpr UIpx operator/(float a) const noexcept {
			return UIpx(math::divide(v, a));
		}
		constexpr UIpx& operator/=(float a) noexcept {
			v = math::divide(v, a);
			return *this;
		}
	};

	//比例（并非百分比）
	class UIrt {
		float v;
	public:
		constexpr explicit UIrt(float v = 0) noexcept : v(v) {}
		constexpr float val() const noexcept {
			return v;
		}

		constexpr UIrt operator+(float a) const noexcept {
			return UIrt(v + a);
		}
		constexpr UIrt& operator+=(float a) noexcept {
			v += a;
			return *this;
		}
		constexpr UIrt operator-(float a) const noexcept {
			return UIrt(v - a);
		}
		constexpr UIrt& operator-=(float a) noexcept {
			v -= a;
			return *this;
		}
		constexpr UIrt operator*(float a) const noexcept {
			return UIrt(v * a);
		}
		constexpr UIrt& operator*=(float a) noexcept {
			v *= a;
			return *this;
		}
		constexpr UIrt operator/(float a) const noexcept {
			return UIrt(math::divide(v, a));
		}
		constexpr UIrt& operator/=(float a) noexcept {
			v = math::divide(v, a);
			return *this;
		}
	};

	//UI单位，像素或比例
	class UIut {
	private:
		bool ipx;
		float v;
	public:
		constexpr explicit UIut(bool isPx, float v = 0) noexcept : ipx(isPx), v(v) {}
		constexpr UIut(UIpx px) noexcept : UIut(true, px.val()) {}
		constexpr UIut(UIrt rt) noexcept : UIut(false, rt.val()) {}
		constexpr UIut& operator=(UIpx px) noexcept {
			ipx = true;
			v = px.val();
			return *this;
		}
		constexpr UIut& operator=(UIrt rt) noexcept {
			ipx = false;
			v = rt.val();
			return *this;
		}

		constexpr bool ispx() const noexcept {
			return ipx;
		}
		constexpr bool isrt() const noexcept {
			return !ipx;
		}
		constexpr float val() const noexcept {
			return v;
		}


		constexpr UIut operator+(float a) const noexcept {
			return UIut(ipx, v + a);
		}
		constexpr UIut& operator+=(float a) noexcept {
			v += a;
			return *this;
		}
		constexpr UIut operator-(float a) const noexcept {
			return UIut(ipx, v - a);
		}
		constexpr UIut& operator-=(float a) noexcept {
			v -= a;
			return *this;
		}
		constexpr UIut operator*(float a) const noexcept {
			return UIut(ipx, v * a);
		}
		constexpr UIut& operator*=(float a) noexcept {
			v *= a;
			return *this;
		}
		constexpr UIut operator/(float a) const noexcept {
			return UIut(ipx, math::divide(v, a));
		}
		constexpr UIut& operator/=(float a) noexcept {
			v = math::divide(v, a);
			return *this;
		}
	};

	namespace literals {
		inline constexpr UIpx operator"" _px(ulong px) {
			return UIpx(static_cast<float>(px));
		}
		inline constexpr UIpx operator"" _px(long double px) {
			return UIpx(static_cast<float>(px));
		}

		inline constexpr UIrt operator"" _rt(ulong rt) {
			return UIrt(static_cast<float>(rt));
		}
		inline constexpr UIrt operator"" _rt(long double rt) {
			return UIrt(static_cast<float>(rt));
		}
	}
	using namespace literals;
}
namespace dme {
	using namespace ui::literals;
}