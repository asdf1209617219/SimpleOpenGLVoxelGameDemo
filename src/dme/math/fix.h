#pragma once
#include <dme/math/vector.h>
namespace dme {
	namespace math {
		struct FixRef;
		struct Fix2;
		struct Fix3;

		struct Abox3f;

		//用于打印小数的小数点后的部分
		inline std::string FloatFractionalStr(float f) noexcept {
			std::string str = std::to_string(f).substr(2);
			// 去除末尾多余的 '0'
			while (!str.empty() && str.back() == '0') {
				str.pop_back();
			}
			return str;
		}
		//用于fix数
		inline constexpr void FixVal(sint& i, float& f) noexcept {
			if (f < 0 || f >= 1) {
				float temp = std::floor(f);
				f -= temp;
				i += static_cast<sint>(temp);
			}
		}
		//用于fix数相减并返回float
		inline constexpr float FixSubToFloat(sint i1, float f1, sint i2, float f2) noexcept {
			return (i1 - i2) + f1 - f2;
		}

		//用于Fix类的比较
		constexpr bool FixLess(sint i1, float f1, sint i2, float f2) noexcept {
			return i1 < i2 || (i1 == i2 && f1 < f2);
		}
		//用于Fix类的比较
		constexpr bool FixLessEqual(sint i1, float f1, sint i2, float f2) noexcept {
			return i1 < i2 || (i1 == i2 && f1 <= f2);
		}
		//用于Fix类的比较
		constexpr bool FixGreater(sint i1, float f1, sint i2, float f2) noexcept {
			return i1 > i2 || (i1 == i2 && f1 > f2);
		}
		//用于Fix类的比较
		constexpr bool FixGreaterEqual(sint i1, float f1, sint i2, float f2) noexcept {
			return i1 > i2 || (i1 == i2 && f1 >= f2);
		}
		//用于Fix类的比较
		constexpr bool FixEqual(sint i1, float f1, sint i2, float f2) noexcept {
			return i1 == i2 && f1 == f2;
		}

		//修正坐标由于浮点数太大时出现的误差，分为整数部分和小数部分，小数部分值在[0,1.0)区间，超过这个值会对整数部分进行加减
		struct Fix {
		private:
			friend FixRef;
			friend Fix2;
			friend Fix3;
			//不允许外部调用该函数，用于节省FixVal函数的调用
			constexpr Fix(float xf, sint x) noexcept : x(x), xf(xf) {}
		public:

			using itype = typename sint;
			using ftype = typename float;
			static constexpr axis_t axis = 1;

			union {
				struct {
					sint x;
					float xf;
				};
				struct {
					sint i;
					float f;
				};
			};

			constexpr Fix() noexcept : x(0), xf(0) {}
			constexpr explicit Fix(sint x) noexcept : x(x), xf(0) {}
			constexpr Fix(sint x, float xf) noexcept : x(x), xf(xf) {
				FixVal(x, xf);
			}

			//如果直接操作了数值，调用该函数进行更新
			constexpr void update() noexcept {
				FixVal(x, xf);
			}

			//获取当前对象减输入参数的向量
			constexpr float vector(const Fix& a) const noexcept {
				sint tx = x - a.x;
				return tx + xf - a.xf;
			}
			//获取与另一个对象之间的中心点
			constexpr Fix center(const Fix& a) const noexcept {
				sint ti = x - a.x;
				return Fix(x + (ti / 2), (xf + a.xf + (ti % 2)) * 0.5f);
			}
			//将整数部与小数部相加，转为float
			constexpr float castToFloat() const noexcept {
				return x + xf;
			}

			//增加x轴
			constexpr Fix plus(float a) const noexcept {
				return Fix(x, xf + a);
			}
			//+=
			constexpr Fix& plusAssignment(float a) noexcept {
				xf += a;
				FixVal(x, xf);
				return *this;
			}

			//减去并返回float
			constexpr float subToFloat(const Fix& a) const noexcept {
				return (x - a.x) + xf - a.xf;
			}

			//倒数
			constexpr Fix inversion() const noexcept {
				return Fix(0, divide(1.0f, x + xf));
			}

			constexpr Fix operator +(const Fix& a) const noexcept {
				return Fix(x + a.x, xf + a.xf);
			}
			constexpr Fix operator +(sint a) const noexcept {
				return Fix(xf, x + a);
			}
			constexpr Fix operator +(float a) const noexcept {
				return Fix(x, xf + a);
			}
			constexpr Fix& operator +=(const Fix& a) noexcept {
				x += a.x;
				xf += a.xf;
				FixVal(x, xf);
				return *this;
			}
			constexpr Fix& operator +=(sint a) noexcept {
				x += a;
				return *this;
			}
			constexpr Fix& operator +=(float a) noexcept {
				xf += a;
				FixVal(x, xf);
				return *this;
			}

			constexpr Fix operator -(const Fix& a) const noexcept {
				return Fix(x - a.x, xf - a.xf);
			}
			constexpr Fix operator -(sint a) const noexcept {
				return Fix(xf, x - a);
			}
			constexpr Fix operator -(float a) const noexcept {
				return Fix(x, xf - a);
			}
			constexpr Fix& operator -=(const Fix& a) noexcept {
				x -= a.x;
				xf -= a.xf;
				FixVal(x, xf);
				return *this;
			}
			constexpr Fix& operator -=(sint a) noexcept {
				x -= a;
				return *this;
			}
			constexpr Fix& operator -=(float a) noexcept {
				xf -= a;
				FixVal(x, xf);
				return *this;
			}

			constexpr Fix operator *(const Fix& a) const noexcept {
				sint ti = x * a.x;
				float tf = xf * (a.xf + a.x) + a.xf * x;
				return Fix(ti, tf);
			}
			constexpr Fix operator *(sint a) const noexcept {
				return Fix(x * a, xf * a);
			}
			constexpr Fix operator *(float a) const noexcept {
				Fix tfix = Fix(0, x * a);
				return Fix(tfix.x, xf * a + tfix.xf);
			}
			constexpr Fix& operator *=(const Fix& a) noexcept {
				xf = xf * (a.xf + a.x) + a.xf * x;
				x *= a.x;
				FixVal(x, xf);
				return *this;
			}
			constexpr Fix& operator *=(sint a) noexcept {
				xf *= a;
				x *= a;
				FixVal(x, xf);
				return *this;
			}
			constexpr Fix& operator *=(float a) noexcept {
				Fix tfix = Fix(0, x * a);
				x = tfix.x;
				xf = xf * a + tfix.xf;
				FixVal(x, xf);
				return *this;
			}

			constexpr Fix operator /(const Fix& a) const noexcept {
				return *this * a.inversion();
			}
			constexpr Fix operator /(sint a) const noexcept {
				return *this * divide(1.0f, a);
			}
			constexpr Fix operator /(float a) const noexcept {
				return *this * divide(1.0f, a);
			}
			constexpr Fix& operator /=(const Fix& a) noexcept {
				return *this *= a.inversion();
			}
			constexpr Fix& operator /=(sint a) noexcept {
				return *this *= divide(1.0f, a);
			}
			constexpr Fix& operator /=(float a) noexcept {
				return *this *= divide(1.0f, a);
			}

			constexpr bool operator ==(const Fix& a) const noexcept {
				return x == a.x && xf == a.xf;
			}
			constexpr bool operator !=(const Fix& a) const noexcept {
				return x != a.x || xf != a.xf;
			}
			constexpr bool operator <(const Fix& a) const noexcept {
				return x < a.x || (x == a.x && xf < a.xf);
			}
			constexpr bool operator <=(const Fix& a) const noexcept {
				return x < a.x || (x == a.x && xf <= a.xf);
			}
			constexpr bool operator >(const Fix& a) const noexcept {
				return x > a.x || (x == a.x && xf > a.xf);
			}
			constexpr bool operator >=(const Fix& a) const noexcept {
				return x > a.x || (x == a.x && xf >= a.xf);
			}
			friend std::ostream& operator<<(std::ostream& os, const Fix& a) {
				//return os << "x: " << a.x << ", xf: " << a.xf;
				return os << "x: " << a.x << "." << FloatFractionalStr(a.xf);
			}
			friend constexpr Hash Hasher(const Fix& a) noexcept {
				return Hash::Combine(a.i, a.f);
			}
		};

		//用于引用Fix2,Fix3的某个轴
		struct FixRef {
		private:
			friend Fix2;
			friend Fix3;
			constexpr FixRef(float& f, sint& i) noexcept : i(i), f(f) {}
		public:
			sint& i;
			float& f;

			//获取当前对象减输入参数的向量
			constexpr float vector(const Fix& a) const noexcept {
				sint ti = i - a.x;
				return ti + f - a.xf;
			}
			//获取与另一个对象之间的中心点
			constexpr Fix center(const Fix& a) const noexcept {
				sint ti = i - a.x;
				return Fix(i + (ti / 2), (f + a.xf + (ti % 2)) * 0.5f);
			}
			//将整数部与小数部相加，转为float
			constexpr float castToFloat() const noexcept {
				return i + f;
			}

			//+
			constexpr Fix plus(float a) const noexcept {
				return Fix(i, f + a);
			}
			//+=
			constexpr FixRef& plusAssignment(float a) noexcept {
				f += a;
				FixVal(i, f);
				return *this;
			}

			//减去并返回float
			constexpr float subToFloat(const Fix& a) const noexcept {
				return (i - a.x) + f - a.xf;
			}

			//倒数
			constexpr Fix inversion() const noexcept {
				return Fix(0, divide(1.0f, i + f));
			}

			constexpr operator Fix() const noexcept {
				return Fix(f, i);
			}

			constexpr FixRef& operator =(const Fix& a) noexcept {
				i = a.x;
				f = a.xf;
				return *this;
			}

			constexpr Fix operator +(const Fix& a) const noexcept {
				return Fix(i + a.x, f + a.xf);
			}
			constexpr Fix operator +(sint a) const noexcept {
				return Fix(f, i + a);
			}
			constexpr Fix operator +(float a) const noexcept {
				return Fix(i, f + a);
			}
			constexpr FixRef& operator +=(const Fix& a) noexcept {
				i += a.x;
				f += a.xf;
				FixVal(i, f);
				return *this;
			}
			constexpr FixRef& operator +=(sint a) noexcept {
				i += a;
				return *this;
			}
			constexpr FixRef& operator +=(float a) noexcept {
				f += a;
				FixVal(i, f);
				return *this;
			}

			constexpr Fix operator -(const Fix& a) const noexcept {
				return Fix(i - a.x, f - a.xf);
			}
			constexpr Fix operator -(sint a) const noexcept {
				return Fix(f, i - a);
			}
			constexpr Fix operator -(float a) const noexcept {
				return Fix(i, f - a);
			}
			constexpr FixRef& operator -=(const Fix& a) noexcept {
				i -= a.x;
				f -= a.xf;
				FixVal(i, f);
				return *this;
			}
			constexpr FixRef& operator -=(sint a) noexcept {
				i -= a;
				return *this;
			}
			constexpr FixRef& operator -=(float a) noexcept {
				f -= a;
				FixVal(i, f);
				return *this;
			}

			constexpr Fix operator *(const Fix& a) const noexcept {
				sint ti = i * a.x;
				float tf = f * (a.xf + a.x) + a.xf * i;
				return Fix(ti, tf);
			}
			constexpr Fix operator *(sint a) const noexcept {
				return Fix(i * a, f * a);
			}
			constexpr Fix operator *(float a) const noexcept {
				Fix tfix = Fix(0, i * a);
				return Fix(tfix.x, f * a + tfix.xf);
			}
			constexpr FixRef& operator *=(const Fix& a) noexcept {
				f = f * (a.xf + a.x) + a.xf * i;
				i *= a.x;
				FixVal(i, f);
				return *this;
			}
			constexpr FixRef& operator *=(sint a) noexcept {
				f *= a;
				i *= a;
				FixVal(i, f);
				return *this;
			}
			constexpr FixRef& operator *=(float a) noexcept {
				Fix tfix = Fix(0, i * a);
				i = tfix.x;
				f = f * a + tfix.xf;
				FixVal(i, f);
				return *this;
			}

			constexpr Fix operator /(const Fix& a) const noexcept {
				return *this * a.inversion();
			}
			constexpr Fix operator /(sint a) const noexcept {
				return *this * divide(1.0f, a);
			}
			constexpr Fix operator /(float a) const noexcept {
				return *this * divide(1.0f, a);
			}
			constexpr FixRef& operator /=(const Fix& a) noexcept {
				return *this *= a.inversion();
			}
			constexpr FixRef& operator /=(sint a) noexcept {
				return *this *= divide(1.0f, a);
			}
			constexpr FixRef& operator /=(float a) noexcept {
				return *this *= divide(1.0f, a);
			}

			constexpr bool operator ==(const Fix& a) const noexcept {
				return i == a.x && f == a.xf;
			}
			constexpr bool operator !=(const Fix& a) const noexcept {
				return i != a.x || f != a.xf;
			}
			constexpr bool operator <(const Fix& a) const noexcept {
				return i < a.x || (i == a.x && f < a.xf);
			}
			constexpr bool operator <=(const Fix& a) const noexcept {
				return i < a.x || (i == a.x && f <= a.xf);
			}
			constexpr bool operator >(const Fix& a) const noexcept {
				return i > a.x || (i == a.x && f > a.xf);
			}
			constexpr bool operator >=(const Fix& a) const noexcept {
				return i > a.x || (i == a.x && f >= a.xf);
			}
			friend std::ostream& operator<<(std::ostream& os, const FixRef& a) {
				//return os << "i: " << a.i << ", f: " << a.f;
				return os << "v: " << a.i << "." << FloatFractionalStr(a.f);
			}
			friend constexpr Hash Hasher(const FixRef& a) noexcept {
				return Hash::Combine(a.i, a.f);
			}
		};

		//修正坐标由于浮点数太大时出现的误差，分为整数部分和小数部分，小数部分值在[0,1.0)区间，超过这个值会对整数部分进行加减
		struct Fix2 {
		private:
			friend Fix3;
			//不允许外部调用该函数，用于节省FixVal函数的调用
			constexpr Fix2(float xf, float yf, sint x, sint y) noexcept : x(x), y(y), xf(xf), yf(yf) {}
			//不允许外部调用该函数，用于节省FixVal函数的调用
			constexpr Fix2(const Float2& af, const Int2& a) noexcept : x(a.x), y(a.y), xf(af.x), yf(af.y) {}
		public:
			using itype = typename sint;
			using ftype = typename float;
			static constexpr axis_t axis = 2;

			union {
				struct {
					sint x;
					sint y;
					float xf;
					float yf;
				};
				struct {
					Int2 i;
					Float2 f;
				};
			};

			constexpr Fix2() noexcept : x(0), y(0), xf(0), yf(0) {}
			constexpr explicit Fix2(sint x) noexcept : x(x), y(x), xf(0), yf(0) {}
			constexpr Fix2(sint x, sint y) noexcept : x(x), y(y), xf(0), yf(0) {}
			constexpr explicit Fix2(Int2 a) noexcept : i(a), f() {}
			constexpr Fix2(sint x_, sint y_, float xf_, float yf_) noexcept : x(x_), y(y_), xf(xf_), yf(yf_) {
				FixVal(x, xf);
				FixVal(y, yf);
			}
			constexpr Fix2(Int2 a, Float2 af) noexcept : i(a), f(af) {
				FixVal(x, xf);
				FixVal(y, yf);
			}

			//如果直接操作了数值，调用该函数进行更新全部值
			constexpr void update() noexcept {
				FixVal(x, xf);
				FixVal(y, yf);
			}
			//如果直接操作了某个轴，调用该函数进行更新
			constexpr void update(axis_t index) noexcept {
				if constexpr (_math_safe_index_) {
					index %= 2;
				}
				FixVal(i[index], f[index]);
			}
			//如果直接操作了x轴，调用该函数进行更新
			constexpr void updateX() noexcept {
				FixVal(x, xf);
			}
			//如果直接操作了y轴，调用该函数进行更新
			constexpr void updateY() noexcept {
				FixVal(y, yf);
			}

			//获取当前对象减输入参数的向量
			constexpr Float2 vector(const Fix2& a) const noexcept {
				Int2 ti = i - a.i;
				return f - a.f + ti;
			}
			//获取与另一个对象之间的中心点
			constexpr Fix2 center(const Fix2& a) const noexcept {
				Int2 ti = i - a.i;
				return Fix2(i + (ti / 2), (f + a.f + (ti % 2)) * 0.5f);
			}
			//将整数部与小数部相加，转为Float3
			constexpr Float2 castToFloat() const noexcept {
				return Float2(x + xf, y + yf);
			}
			//丢弃某个轴
			constexpr Fix discard(axis_t index) const noexcept {
				index = (~index) & 1;
				return Fix(f[index], i[index]);
			}

			//只增加x轴
			constexpr Fix2 plusX(float a) const noexcept {
				Fix2 r = Fix2(xf + a, yf, x, y);
				FixVal(r.x, r.xf);
				return r;
			}
			//+=
			constexpr Fix2& plusAssignmentX(float a) noexcept {
				xf += a;
				FixVal(x, xf);
				return *this;
			}
			//减去x轴并返回float
			constexpr float subXToFloat(const Fix2& a) const noexcept {
				return (x - a.x) + xf - a.xf;
			}
			//判断x轴是否小于输入参数的x轴
			constexpr bool lessX(const Fix2& a) const noexcept {
				return x < a.x || (x == a.x && xf < a.xf);
			}
			//判断x轴是否小于等于输入参数的x轴
			constexpr bool lessEqualX(const Fix2& a) const noexcept {
				return x < a.x || (x == a.x && xf <= a.xf);
			}
			//判断x轴是否大于输入参数的x轴
			constexpr bool greaterX(const Fix2& a) const noexcept {
				return x > a.x || (x == a.x && xf > a.xf);
			}
			//判断x轴是否大于等于输入参数的x轴
			constexpr bool greaterEqualX(const Fix2& a) const noexcept {
				return x > a.x || (x == a.x && xf >= a.xf);
			}
			//判断x轴是否等于输入参数的x轴
			constexpr bool equalX(const Fix2& a) const noexcept {
				return x == a.x && xf == a.xf;
			}

			//只增加y轴
			constexpr Fix2 plusY(float a) const noexcept {
				Fix2 r = Fix2(xf, yf + a, x, y);
				FixVal(r.y, r.yf);
				return r;
			}
			//+=
			constexpr Fix2& plusAssignmentY(float a) noexcept {
				yf += a;
				FixVal(y, yf);
				return *this;
			}
			//减去y轴并返回float
			constexpr float subYToFloat(const Fix2& a) const noexcept {
				return (y - a.y) + yf - a.yf;
			}
			//判断y轴是否小于输入参数的y轴
			constexpr bool lessY(const Fix2& a) const noexcept {
				return y < a.y || (y == a.y && yf < a.yf);
			}
			//判断y轴是否小于等于输入参数的y轴
			constexpr bool lessEqualY(const Fix2& a) const noexcept {
				return y < a.y || (y == a.y && yf <= a.yf);
			}
			//判断y轴是否大于输入参数的y轴
			constexpr bool greaterY(const Fix2& a) const noexcept {
				return y > a.y || (y == a.y && yf > a.yf);
			}
			//判断y轴是否大于等于输入参数的y轴
			constexpr bool greaterEqualY(const Fix2& a) const noexcept {
				return y > a.y || (y == a.y && yf >= a.yf);
			}
			//判断y轴是否等于输入参数的y轴
			constexpr bool equalY(const Fix2& a) const noexcept {
				return y == a.y && yf == a.yf;
			}

			//增加对应轴
			constexpr Fix2 plus(axis_t index, float a) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 2;
				}
				Fix2 r = Fix2(xf, yf, x, y);
				r.f[index] += a;
				FixVal(r.i[index], r.f[index]);
				return r;
			}
			//+=
			constexpr Fix2& plusAssignment(axis_t index, float a) noexcept {
				if constexpr (_math_safe_index_) {
					index %= 2;
				}
				f[index] += a;
				FixVal(i[index], f[index]);
				return *this;
			}
			//减去对应轴并返回float
			constexpr float subToFloat(axis_t index, const Fix2& a) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 2;
				}
				return (i[index] - a.i[index]) + f[index] - a.f[index];
			}
			//判断对应轴是否小于输入参数的对应轴
			constexpr bool less(axis_t index, const Fix2& a) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 2;
				}
				return i[index] < a.i[index] || (i[index] == a.i[index] && f[index] < a.f[index]);
			}
			//判断对应轴是否小于等于输入参数的对应轴
			constexpr bool lessEqual(axis_t index, const Fix2& a) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 2;
				}
				return i[index] < a.i[index] || (i[index] == a.i[index] && f[index] <= a.f[index]);
			}
			//判断对应轴是否大于输入参数的对应轴
			constexpr bool greater(axis_t index, const Fix2& a) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 2;
				}
				return i[index] > a.i[index] || (i[index] == a.i[index] && f[index] > a.f[index]);
			}
			//判断对应轴是否大于等于输入参数的对应轴
			constexpr bool greaterEqual(axis_t index, const Fix2& a) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 2;
				}
				return i[index] > a.i[index] || (i[index] == a.i[index] && f[index] >= a.f[index]);
			}
			//判断对应轴是否等于输入参数的对应轴
			constexpr bool equal(axis_t index, const Fix2& a) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 2;
				}
				return i[index] == a.i[index] && f[index] == a.f[index];
			}


			//令对应轴与某个数的相等
			constexpr void set(axis_t index, sint a) noexcept {
				if constexpr (_math_safe_index_) {
					index %= 2;
				}
				i[index] = a;
				f[index] = 0;
			}
			//令对应轴的整数和小数分别与某两个数的相等
			constexpr void set(axis_t index, sint a, float af) noexcept {
				if constexpr (_math_safe_index_) {
					index %= 2;
				}
				i[index] = a;
				f[index] = af;
				FixVal(i[index], f[index]);
			}
			//令对应轴与其他数的对应轴相等
			constexpr void set(axis_t index, const Fix2& a) noexcept {
				if constexpr (_math_safe_index_) {
					index %= 2;
				}
				i[index] = a.i[index];
				f[index] = a.f[index];
			}

			//引用x轴
			constexpr FixRef refX() noexcept {
				return FixRef(xf, x);
			}
			//引用y轴
			constexpr FixRef refY() noexcept {
				return FixRef(yf, y);
			}
			//引用对应下标的轴
			constexpr FixRef ref(axis_t index) noexcept {
				if constexpr (_math_safe_index_) {
					index %= 2;
				}
				return FixRef(f[index], i[index]);
			}
			//获取x轴
			constexpr Fix getX() const noexcept {
				return Fix(xf, x);
			}
			//获取y轴
			constexpr Fix getY() const noexcept {
				return Fix(yf, y);
			}
			//获取对应下标的轴
			constexpr Fix get(axis_t index) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 2;
				}
				return Fix(f[index], i[index]);
			}


			//返回距离（注意由于调用了std::sqrt所以是非constexpr的）
			float distance(const Fix2& a) const noexcept {
				float x_ = (a.x - x) + a.xf - xf;
				float y_ = (a.y - y) + a.yf - yf;
				return std::sqrt(x_ * x_ + y_ * y_);
			}

			//倒数
			constexpr Fix2 inversion() const noexcept {
				return Fix2(0, 0, divide(1.0f, x + xf), divide(1.0f, y + yf));
			}

			//求点p0在线段p1p2的什么位置，如果返回数（即p1p2与p1p0的叉乘的差）大于0，则表示点在线段顺时针方向，小于0为逆时针，等于0为在线段上
			constexpr static float DirectionAtLine(const Fix2& p1, const Fix2& p2, const Int2& p0) noexcept {
				/*
				float x1 = tof(p2.x - p1.x) + p2.xf - p1.xf;
				float y1 = tof(p2.y - p1.y) + p2.yf - p2.yf;
				float x2 = tof(p0.x - p1.x) - p1.xf;
				float y2 = tof(p0.y - p1.y) - p1.yf;
				return x1 * y2 - x2 * y1;
				*/
				return ((p2.x - p1.x) + p2.xf - p1.xf)
					* ((p0.y - p1.y) - p1.yf)
					- ((p0.x - p1.x) - p1.xf)
					* ((p2.y - p1.y) + p2.yf - p2.yf);
			}
			constexpr static float DirectionAtLine(sint p1x, float p1xf, sint p1y, float p1yf, sint p2x, float p2xf, sint p2y, float p2yf, sint p0x, sint p0y) noexcept {
				return ((p2x - p1x) + p2xf - p1xf)
					* ((p0y - p1y) - p1yf)
					- ((p0x - p1x) - p1xf)
					* ((p2y - p1y) + p2yf - p2yf);
			}

			constexpr Fix2 operator +(const Fix2& a) const noexcept {
				return Fix2(i + a.i, f + a.f);
			}
			constexpr Fix2 operator +(const Int2& a) const noexcept {
				return Fix2(f, i + a);
			}
			constexpr Fix2 operator +(const Float2& a) const noexcept {
				return Fix2(i, f + a);
			}
			constexpr Fix2 operator +(sint a) const noexcept {
				return Fix2(f, i + a);
			}
			constexpr Fix2 operator +(float a) const noexcept {
				return Fix2(i, f + a);
			}
			constexpr Fix2& operator +=(const Fix2& a) noexcept {
				i += a.i;
				f += a.f;
				update();
				return *this;
			}
			constexpr Fix2& operator +=(const Int2& a) noexcept {
				i += a;
				return *this;
			}
			constexpr Fix2& operator +=(const Float2& a) noexcept {
				f += a;
				update();
				return *this;
			}
			constexpr Fix2& operator +=(sint a) noexcept {
				i += a;
				return *this;
			}
			constexpr Fix2& operator +=(float a) noexcept {
				f += a;
				update();
				return *this;
			}

			constexpr Fix2 operator -(const Fix2& a) const noexcept {
				return Fix2(i - a.i, f - a.f);
			}
			constexpr Fix2 operator -(const Int2& a) const noexcept {
				return Fix2(f, i - a);
			}
			constexpr Fix2 operator -(const Float2& a) const noexcept {
				return Fix2(i, f - a);
			}
			constexpr Fix2 operator -(sint a) const noexcept {
				return Fix2(f, i - a);
			}
			constexpr Fix2 operator -(float a) const noexcept {
				return Fix2(i, f - a);
			}
			constexpr Fix2& operator -=(const Fix2& a) noexcept {
				i -= a.i;
				f -= a.f;
				update();
				return *this;
			}
			constexpr Fix2& operator -=(const Int2& a) noexcept {
				i -= a;
				return *this;
			}
			constexpr Fix2& operator -=(const Float2& a) noexcept {
				f -= a;
				update();
				return *this;
			}
			constexpr Fix2& operator -=(sint a) noexcept {
				i -= a;
				return *this;
			}
			constexpr Fix2& operator -=(float a) noexcept {
				f -= a;
				update();
				return *this;
			}

			constexpr Fix2 operator *(const Fix2& a) const noexcept {
				Int2 ti = i * a.i;
				Float2 tf = f * (a.f + a.i) + a.f * i;
				return Fix2(ti, tf);
			}
			constexpr Fix2 operator *(const Int2& a) const noexcept {
				return Fix2(i * a, f * a);
			}
			constexpr Fix2 operator *(const Float2& a) const noexcept {
				Fix2 t = Fix2(Int2(), i * a);
				return Fix2(t.i, f * a + t.f);
			}
			constexpr Fix2 operator *(sint a) const noexcept {
				return Fix2(i * a, f * a);
			}
			constexpr Fix2 operator *(float a) const noexcept {
				Fix2 t = Fix2(Int2(), i * a);
				return Fix2(t.i, f * a + t.f);
			}
			constexpr Fix2& operator *=(const Fix2& a) noexcept {
				f = f * (a.f + a.i) + a.f * i;
				i = i * a.i;
				update();
				return *this;
			}
			constexpr Fix2& operator *=(const Int2& a) noexcept {
				i *= a;
				f *= a;
				update();
				return *this;
			}
			constexpr Fix2& operator *=(const Float2& a) noexcept {
				Fix2 t = Fix2(Int2(), i * a);
				i = t.i;
				f = f * a + t.f;
				update();
				return *this;
			}
			constexpr Fix2& operator *=(sint a) noexcept {
				i *= a;
				f *= a;
				update();
				return *this;
			}
			constexpr Fix2& operator *=(float a) noexcept {
				Fix2 t = Fix2(Int2(), i * a);
				i = t.i;
				f = f * a + t.f;
				update();
				return *this;
			}

			constexpr Fix2 operator /(const Fix2& a) const noexcept {
				return *this * a.inversion();
			}
			constexpr Fix2 operator /(const Int2& a) const noexcept {
				return *this * Float2(divide(1.0f, a.x), divide(1.0f, a.y));
			}
			constexpr Fix2 operator /(const Float2& a) const noexcept {
				return *this * a.inversion();
			}
			constexpr Fix2 operator /(sint a) const noexcept {
				return *this * divide(1.0f, a);
			}
			constexpr Fix2 operator /(float a) const noexcept {
				return *this * divide(1.0f, a);
			}
			constexpr Fix2& operator /=(const Fix2& a) noexcept {
				return *this *= a.inversion();
			}
			constexpr Fix2& operator /=(const Int2& a) noexcept {
				return *this *= Float2(divide(1.0f, a.x), divide(1.0f, a.y));
			}
			constexpr Fix2& operator /=(const Float2& a) noexcept {
				return *this *= a.inversion();
			}
			constexpr Fix2& operator /=(sint a) noexcept {
				return *this *= divide(1.0f, a);
			}
			constexpr Fix2& operator /=(float a) noexcept {
				return *this *= divide(1.0f, a);
			}

			constexpr bool operator ==(const Fix2& a) const noexcept {
				return x == a.x && y == a.y && xf == a.xf && yf == a.yf;
			}
			constexpr bool operator !=(const Fix2& a) const noexcept {
				return x != a.x || y != a.y || xf != a.xf || yf != a.yf;
			}
			friend std::ostream& operator<<(std::ostream& os, const Fix2& a) {
				//return os << "x: " << a.x << ", xf: " << a.xf << ", y: " << a.y << ", yf: " << a.yf;
				return os << "x: " << a.x << "." << FloatFractionalStr(a.xf)
					<< ", y: " << a.y << "." << FloatFractionalStr(a.yf);
			}
			friend constexpr Hash Hasher(const Fix2& a) noexcept {
				return Hash::Combine(a.i, a.f);
			}
		};

		//修正坐标由于浮点数太大时出现的误差，分为整数部分和小数部分，小数部分值在[0,1.0)区间，超过这个值会对整数部分进行加减
		struct Fix3 {
		private:
			friend Abox3f;

			//不允许外部调用该函数，用于节省FixVal函数的调用
			constexpr Fix3(float xf, float yf, float zf, sint x, sint y, sint z) noexcept : x(x), y(y), z(z), xf(xf), yf(yf), zf(zf) {}
			//不允许外部调用该函数，用于节省FixVal函数的调用
			constexpr Fix3(const Float3& af, const Int3& a) noexcept : i(a), f(af) {}
		public:
			using itype = typename sint;
			using ftype = typename float;
			static constexpr axis_t axis = 3;

			union {
				struct {
					sint x;
					sint y;
					sint z;
					float xf;
					float yf;
					float zf;
				};
				struct {
					Int3 i;
					Float3 f;
				};
			};

			constexpr Fix3() noexcept : x(0), y(0), z(0), xf(0), yf(0), zf(0) {};
			constexpr explicit Fix3(sint x) noexcept : x(x), y(x), z(x), xf(0), yf(0), zf(0) {}
			constexpr Fix3(sint x, sint y, sint z) noexcept : x(x), y(y), z(z), xf(0), yf(0), zf(0) {}
			constexpr explicit Fix3(Int3 a) noexcept : i(a), f() {}
			constexpr Fix3(sint x_, sint y_, sint z_, float xf_, float yf_, float zf_) noexcept : x(x_), y(y_), z(z_), xf(xf_), yf(yf_), zf(zf_) {
				FixVal(x, xf);
				FixVal(y, yf);
				FixVal(z, zf);
			}
			constexpr Fix3(Int3 a, Float3 af) noexcept : i(a), f(af) {
				FixVal(x, xf);
				FixVal(y, yf);
				FixVal(z, zf);
			}

			//如果直接操作了数值，调用该函数进行更新所有轴
			constexpr void update() noexcept {
				FixVal(x, xf);
				FixVal(y, yf);
				FixVal(z, zf);
			}
			//如果直接操作了某个轴，调用该函数进行更新
			constexpr void update(axis_t index) noexcept {
				if constexpr (_math_safe_index_) {
					index %= 3;
				}
				FixVal(i[index], f[index]);
			}
			//如果直接操作了x轴，调用该函数进行更新
			constexpr void updateX() noexcept {
				FixVal(x, xf);
			}
			//如果直接操作了y轴，调用该函数进行更新
			constexpr void updateY() noexcept {
				FixVal(y, yf);
			}
			//如果直接操作了z轴，调用该函数进行更新
			constexpr void updateZ() noexcept {
				FixVal(z, zf);
			}

			//获取当前对象减输入参数的向量
			constexpr Float3 vector(const Fix3& a) const noexcept {
				Int3 ti = i - a.i;
				return f - a.f + ti;
			}
			//获取与另一个对象之间的中心点
			constexpr Fix3 center(const Fix3& a) const noexcept {
				Int3 ti = i - a.i;
				return Fix3(i + (ti / 2), (f + a.f + (ti % 2)) * 0.5f);
			}
			//丢弃某个轴
			constexpr Fix2 discard(axis_t index) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 3;
				}
				switch (index) {
				case 0:
					return Fix2(yf, zf, y, z);
				case 1:
					return Fix2(xf, zf, x, z);
				case 2:
					return Fix2(xf, yf, x, y);
				default:
					return Fix2(xf, yf, x, y);
				}
			}

			//舍弃z维度
			constexpr Fix2 toFix2() const noexcept {
				return Fix2(xf, yf, x, y);
			}

			//将整数部与小数部相加，转为Float3
			constexpr Float3 castToFloat() const noexcept {
				return Float3(x + xf, y + yf, z + zf);
			}

			//只增加x轴
			constexpr Fix3 plusX(float a) const noexcept {
				Fix3 r = Fix3(xf + a, yf, zf, x, y, z);
				FixVal(r.x, r.xf);
				return r;
			}
			//+=
			constexpr Fix3& plusAssignmentX(float a) noexcept {
				xf += a;
				FixVal(x, xf);
				return *this;
			}
			//减去x轴并返回float
			constexpr float subXToFloat(const Fix3& a) const noexcept {
				return (x - a.x) + xf - a.xf;
			}
			//判断x轴是否小于输入参数的x轴
			constexpr bool lessX(const Fix3& a) const noexcept {
				return x < a.x || (x == a.x && xf < a.xf);
			}
			//判断x轴是否小于等于输入参数的x轴
			constexpr bool lessEqualX(const Fix3& a) const noexcept {
				return x < a.x || (x == a.x && xf <= a.xf);
			}
			//判断x轴是否大于输入参数的x轴
			constexpr bool greaterX(const Fix3& a) const noexcept {
				return x > a.x || (x == a.x && xf > a.xf);
			}
			//判断x轴是否大于等于输入参数的x轴
			constexpr bool greaterEqualX(const Fix3& a) const noexcept {
				return x > a.x || (x == a.x && xf >= a.xf);
			}
			//判断x轴是否等于输入参数的x轴
			constexpr bool equalX(const Fix3& a) const noexcept {
				return x == a.x && xf == a.xf;
			}

			//只增加y轴
			constexpr Fix3 plusY(float a) const noexcept {
				Fix3 r = Fix3(xf, yf + a, zf, x, y, z);
				FixVal(r.y, r.yf);
				return r;
			}
			//+=
			constexpr Fix3& plusAssignmentY(float a) noexcept {
				yf += a;
				FixVal(y, yf);
				return *this;
			}
			//减去y轴并返回float
			constexpr float subYToFloat(const Fix3& a) const noexcept {
				return (y - a.y) + yf - a.yf;
			}
			//判断y轴是否小于输入参数的y轴
			constexpr bool lessY(const Fix3& a) const noexcept {
				return y < a.y || (y == a.y && yf < a.yf);
			}
			//判断y轴是否小于等于输入参数的y轴
			constexpr bool lessEqualY(const Fix3& a) const noexcept {
				return y < a.y || (y == a.y && yf <= a.yf);
			}
			//判断y轴是否大于输入参数的y轴
			constexpr bool greaterY(const Fix3& a) const noexcept {
				return y > a.y || (y == a.y && yf > a.yf);
			}
			//判断y轴是否大于等于输入参数的y轴
			constexpr bool greaterEqualY(const Fix3& a) const noexcept {
				return y > a.y || (y == a.y && yf >= a.yf);
			}
			//判断y轴是否等于输入参数的y轴
			constexpr bool equalY(const Fix3& a) const noexcept {
				return y == a.y && yf == a.yf;
			}

			//只增加z轴
			constexpr Fix3 plusZ(float a) const noexcept {
				Fix3 r = Fix3(xf, yf, zf + a, x, y, z);
				FixVal(r.z, r.zf);
				return r;
			}
			//+=
			constexpr Fix3& plusAssignmentZ(float a) noexcept {
				zf += a;
				FixVal(z, zf);
				return *this;
			}
			//减去z轴并返回float
			constexpr float subZToFloat(const Fix3& a) const noexcept {
				return (z - a.z) + zf - a.zf;
			}
			//判断z轴是否小于输入参数的z轴
			constexpr bool lessZ(const Fix3& a) const noexcept {
				return z < a.z || (z == a.z && zf < a.zf);
			}
			//判断z轴是否小于等于输入参数的z轴
			constexpr bool lessEqualZ(const Fix3& a) const noexcept {
				return z < a.z || (z == a.z && zf <= a.zf);
			}
			//判断z轴是否大于输入参数的z轴
			constexpr bool greaterZ(const Fix3& a) const noexcept {
				return z > a.z || (z == a.z && zf > a.zf);
			}
			//判断z轴是否大于等于输入参数的z轴
			constexpr bool greaterEqualZ(const Fix3& a) const noexcept {
				return z > a.z || (z == a.z && zf >= a.zf);
			}
			//判断z轴是否等于输入参数的z轴
			constexpr bool equalZ(const Fix3& a) const noexcept {
				return z == a.z && zf == a.zf;
			}

			//增加对应轴
			constexpr Fix3 plus(axis_t index, float a) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 3;
				}
				Fix3 r = Fix3(xf, yf, zf, x, y, z);
				r.f[index] += a;
				FixVal(r.i[index], r.f[index]);
				return r;
			}
			//+=对应轴
			constexpr Fix3& plusAssignment(axis_t index, float a) noexcept {
				if constexpr (_math_safe_index_) {
					index %= 3;
				}
				f[index] += a;
				FixVal(i[index], f[index]);
				return *this;
			}
			//减去对应轴并返回float
			constexpr float subToFloat(axis_t index, const Fix3& a) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 3;
				}
				return (i[index] - a.i[index]) + f[index] - a.f[index];
			}
			//判断对应轴是否小于输入参数的对应轴
			constexpr bool less(axis_t index, const Fix3& a) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 3;
				}
				return i[index] < a.i[index] || (i[index] == a.i[index] && f[index] < a.f[index]);
			}
			//判断对应轴是否小于等于输入参数的对应轴
			constexpr bool lessEqual(axis_t index, const Fix3& a) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 3;
				}
				return i[index] < a.i[index] || (i[index] == a.i[index] && f[index] <= a.f[index]);
			}
			//判断对应轴是否大于输入参数的对应轴
			constexpr bool greater(axis_t index, const Fix3& a) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 3;
				}
				return i[index] > a.i[index] || (i[index] == a.i[index] && f[index] > a.f[index]);
			}
			//判断对应轴是否大于等于输入参数的对应轴
			constexpr bool greaterEqual(axis_t index, const Fix3& a) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 3;
				}
				return i[index] > a.i[index] || (i[index] == a.i[index] && f[index] >= a.f[index]);
			}
			//判断对应轴是否等于输入参数的对应轴
			constexpr bool equal(axis_t index, const Fix3& a) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 3;
				}
				return i[index] == a.i[index] && f[index] == a.f[index];
			}

			//令对应轴与第二个参数的相等
			constexpr void set(axis_t index, sint a) noexcept {
				if constexpr (_math_safe_index_) {
					index %= 3;
				}
				i[index] = a;
				f[index] = 0;
			}
			//令对应轴的整数和小数分别与第二个参数和第三个参数相等
			constexpr void set(axis_t index, sint a, float af) noexcept {
				if constexpr (_math_safe_index_) {
					index %= 3;
				}
				i[index] = a;
				f[index] = af;
				FixVal(i[index], f[index]);
			}
			//令对应轴与其他数的对应轴相等
			constexpr void set(axis_t index, const Fix3& a) noexcept {
				if constexpr (_math_safe_index_) {
					index %= 3;
				}
				i[index] = a.i[index];
				f[index] = a.f[index];
			}


			//引用x轴
			constexpr FixRef refX() noexcept {
				return FixRef(xf, x);
			}
			//引用y轴
			constexpr FixRef refY() noexcept {
				return FixRef(yf, y);
			}
			//引用z轴
			constexpr FixRef refZ() noexcept {
				return FixRef(zf, z);
			}
			//引用对应下标的轴
			constexpr FixRef ref(axis_t index) noexcept {
				if constexpr (_math_safe_index_) {
					index %= 3;
				}
				return FixRef(f[index], i[index]);
			}
			//获取x轴
			constexpr Fix getX() const noexcept {
				return Fix(xf, x);
			}
			//获取y轴
			constexpr Fix getY() const noexcept {
				return Fix(yf, y);
			}
			//获取z轴
			constexpr Fix getZ() const noexcept {
				return Fix(zf, z);
			}
			//获取对应下标的轴
			constexpr Fix get(axis_t index) const noexcept {
				if constexpr (_math_safe_index_) {
					index %= 3;
				}
				return Fix(f[index], i[index]);
			}
			//获取对应下标的轴
			constexpr Fix2 get(axis_t index1, axis_t index2) const noexcept {
				if constexpr (_math_safe_index_) {
					index1 %= 3;
					index2 %= 3;
				}
				return Fix2(f[index1], f[index2], i[index1], i[index2]);
			}

			//返回距离（注意由于调用了std::sqrt所以是非constexpr的）
			float distance(const Fix3& a) const noexcept {
				float x_ = tof(a.x - x) + a.xf - xf;
				float y_ = tof(a.y - y) + a.yf - yf;
				float z_ = tof(a.z - z) + a.zf - zf;
				return std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
			}

			//倒数
			constexpr Fix3 inversion() const noexcept {
				return Fix3(0, 0, 0, divide(1.0f, x + xf), divide(1.0f, y + yf), divide(1.0f, z + zf));
			}

			constexpr Fix3 operator +(const Fix3& a) const noexcept {
				return Fix3(i + a.i, f + a.f);
			}
			constexpr Fix3 operator +(const Int3& a) const noexcept {
				return Fix3(f, i + a);
			}
			constexpr Fix3 operator +(const Float3& a) const noexcept {
				return Fix3(i, f + a);
			}
			constexpr Fix3 operator +(sint a) const noexcept {
				return Fix3(f, i + a);
			}
			constexpr Fix3 operator +(float a) const noexcept {
				return Fix3(i, f + a);
			}
			constexpr Fix3& operator +=(const Fix3& a) noexcept {
				i += a.i;
				f += a.f;
				update();
				return *this;
			}
			constexpr Fix3& operator +=(const Int3& a) noexcept {
				i += a;
				return *this;
			}
			constexpr Fix3& operator +=(const Float3& a) noexcept {
				f += a;
				update();
				return *this;
			}
			constexpr Fix3& operator +=(sint a) noexcept {
				i += a;
				return *this;
			}
			constexpr Fix3& operator +=(float a) noexcept {
				f += a;
				update();
				return *this;
			}

			constexpr Fix3 operator -(const Fix3& a) const noexcept {
				return Fix3(i - a.i, f - a.f);
			}
			constexpr Fix3 operator -(const Int3& a) const noexcept {
				return Fix3(f, i - a);
			}
			constexpr Fix3 operator -(const Float3& a) const noexcept {
				return Fix3(i, f - a);
			}
			constexpr Fix3 operator -(sint a) const noexcept {
				return Fix3(f, i - a);
			}
			constexpr Fix3 operator -(float a) const noexcept {
				return Fix3(i, f - a);
			}
			constexpr Fix3& operator -=(const Fix3& a) noexcept {
				i -= a.i;
				f -= a.f;
				update();
				return *this;
			}
			constexpr Fix3& operator -=(const Int3& a) noexcept {
				i -= a;
				return *this;
			}
			constexpr Fix3& operator -=(const Float3& a) noexcept {
				f -= a;
				update();
				return *this;
			}
			constexpr Fix3& operator -=(sint a) noexcept {
				i -= a;
				return *this;
			}
			constexpr Fix3& operator -=(float a) noexcept {
				f -= a;
				update();
				return *this;
			}

			constexpr Fix3 operator *(const Fix3& a) const noexcept {
				Int3 i3 = i * a.i;
				Float3 f3 = f * (a.f + a.i) + a.f * i;
				return Fix3(i3, f3);
			}
			constexpr Fix3 operator *(const Int3& a) const noexcept {
				return Fix3(i * a, f * a);
			}
			constexpr Fix3 operator *(const Float3& a) const noexcept {
				Fix3 f3 = Fix3(Int3(), i * a);
				return Fix3(f3.i, f * a + f3.f);
			}
			constexpr Fix3 operator *(sint a) const noexcept {
				return Fix3(i * a, f * a);
			}
			constexpr Fix3 operator *(float a) const noexcept {
				Fix3 f3 = Fix3(Int3(), i * a);
				return Fix3(f3.i, f * a + f3.f);
			}
			constexpr Fix3& operator *=(const Fix3& a) noexcept {
				f = f * (a.f + a.i) + a.f * i;
				i = i * a.i;
				update();
				return *this;
			}
			constexpr Fix3& operator *=(const Int3& a) noexcept {
				i *= a;
				f *= a;
				update();
				return *this;
			}
			constexpr Fix3& operator *=(const Float3& a) noexcept {
				Fix3 f3 = Fix3(Int3(), i * a);
				i = f3.i;
				f = f * a + f3.f;
				update();
				return *this;
			}
			constexpr Fix3& operator *=(sint a) noexcept {
				i *= a;
				f *= a;
				update();
				return *this;
			}
			constexpr Fix3& operator *=(float a) noexcept {
				Fix3 f3 = Fix3(Int3(), i * a);
				i = f3.i;
				f = f * a + f3.f;
				update();
				return *this;
			}

			constexpr Fix3 operator /(const Fix3& a) const noexcept {
				return *this * a.inversion();
			}
			constexpr Fix3 operator /(const Int3& a) const noexcept {
				return *this * Float3(divide(1.0f, a.x), divide(1.0f, a.y), divide(1.0f, a.z));
			}
			constexpr Fix3 operator /(const Float3& a) const noexcept {
				return *this * a.inversion();
			}
			constexpr Fix3 operator /(sint a) const noexcept {
				return *this * divide(1.0f, a);
			}
			constexpr Fix3 operator /(float a) const noexcept {
				return *this * divide(1.0f, a);
			}
			constexpr Fix3& operator /=(const Fix3& a) noexcept {
				return *this *= a.inversion();
			}
			constexpr Fix3& operator /=(const Int3& a) noexcept {
				return *this *= Float3(divide(1.0f, a.x), divide(1.0f, a.y), divide(1.0f, a.z));
			}
			constexpr Fix3& operator /=(const Float3& a) noexcept {
				return *this *= a.inversion();
			}
			constexpr Fix3& operator /=(sint a) noexcept {
				return *this *= divide(1.0f, a);
			}
			constexpr Fix3& operator /=(float a) noexcept {
				return *this *= divide(1.0f, a);
			}
		
			constexpr bool operator ==(const Fix3& a) const noexcept {
				return x == a.x && y == a.y && z == a.z && xf == a.xf && yf == a.yf && zf == a.zf;
			}
			constexpr bool operator !=(const Fix3& a) const noexcept {
				return x != a.x || y != a.y || z != a.z || xf != a.xf || yf != a.yf || zf != a.zf;
			}
			friend std::ostream& operator<<(std::ostream& os, const Fix3& a) {
				//return os << "x: " << a.x << ", xf: " << a.xf << ", y: " << a.y << ", yf: " << a.yf << ", z: " << a.z << ", zf: " << a.zf;
				return os << "x: " << a.x << "." << FloatFractionalStr(a.xf)
					<< ", y: " << a.y << "." << FloatFractionalStr(a.yf)
					<< ", z: " << a.z << "." << FloatFractionalStr(a.zf);
			}
			friend constexpr Hash Hasher(const Fix3& a) noexcept {
				return Hash::Combine(a.i, a.f);
			}
		};

		template<>
		DME_INLINE constexpr auto Min(const Fix& a, const Fix& b) noexcept {
			Fix r = Fix();
			if (a.x < b.x || (a.x == b.x && a.xf < b.xf)) {
				r.x = a.x;
				r.xf = a.xf;
			}
			else {
				r.x = b.x;
				r.xf = b.xf;
			}
			return r;
		}

		template<>
		DME_INLINE constexpr auto Max(const Fix& a, const Fix& b) noexcept {
			Fix r = Fix();
			if (a.x > b.x || (a.x == b.x && a.xf > b.xf)) {
				r.x = a.x;
				r.xf = a.xf;
			}
			else {
				r.x = b.x;
				r.xf = b.xf;
			}
			return r;
		}

		template<>
		DME_INLINE constexpr auto Min(const Fix2& a, const Fix2& b) noexcept {
			Fix2 r = Fix2();
			if (a.x < b.x || (a.x == b.x && a.xf < b.xf)) {
				r.x = a.x;
				r.xf = a.xf;
			}
			else {
				r.x = b.x;
				r.xf = b.xf;
			}
			if (a.y < b.y || (a.y == b.y && a.yf < b.yf)) {
				r.y = a.y;
				r.yf = a.yf;
			}
			else {
				r.y = b.y;
				r.yf = b.yf;
			}
			return r;
		}
		template<>
		DME_INLINE constexpr auto Max(const Fix2& a, const Fix2& b) noexcept {
			Fix2 r = Fix2();
			if (a.x > b.x || (a.x == b.x && a.xf > b.xf)) {
				r.x = a.x;
				r.xf = a.xf;
			}
			else {
				r.x = b.x;
				r.xf = b.xf;
			}
			if (a.y > b.y || (a.y == b.y && a.yf > b.yf)) {
				r.y = a.y;
				r.yf = a.yf;
			}
			else {
				r.y = b.y;
				r.yf = b.yf;
			}
			return r;
		}

		template<>
		DME_INLINE constexpr auto Min(const Fix3& a, const Fix3& b) noexcept {
			Fix3 r = Fix3();
			if (a.x < b.x || (a.x == b.x && a.xf < b.xf)) {
				r.x = a.x;
				r.xf = a.xf;
			}
			else {
				r.x = b.x;
				r.xf = b.xf;
			}
			if (a.y < b.y || (a.y == b.y && a.yf < b.yf)) {
				r.y = a.y;
				r.yf = a.yf;
			}
			else {
				r.y = b.y;
				r.yf = b.yf;
			}
			if (a.z < b.z || (a.z == b.z && a.zf < b.zf)) {
				r.z = a.z;
				r.zf = a.zf;
			}
			else {
				r.z = b.z;
				r.zf = b.zf;
			}
			return r;
		}
		template<>
		DME_INLINE constexpr auto Max(const Fix3& a, const Fix3& b) noexcept {
			Fix3 r = Fix3();
			if (a.x > b.x || (a.x == b.x && a.xf > b.xf)) {
				r.x = a.x;
				r.xf = a.xf;
			}
			else {
				r.x = b.x;
				r.xf = b.xf;
			}
			if (a.y > b.y || (a.y == b.y && a.yf > b.yf)) {
				r.y = a.y;
				r.yf = a.yf;
			}
			else {
				r.y = b.y;
				r.yf = b.yf;
			}
			if (a.z > b.z || (a.z == b.z && a.zf > b.zf)) {
				r.z = a.z;
				r.zf = a.zf;
			}
			else {
				r.z = b.z;
				r.zf = b.zf;
			}
			return r;
		}

		//两个Fix3在整数部分和小数部分分开保存的情况下进行比较
		DME_INLINE constexpr auto Min(const Int3& ai, const Float3& af, const Int3& bi, const Float3& bf) noexcept {
			Fix3 r = Fix3();
			if (ai.x < bi.x || (ai.x == bi.x && af.x < bf.x)) {
				r.x = ai.x;
				r.xf = af.x;
			}
			else {
				r.x = bi.x;
				r.xf = bf.x;
			}
			if (ai.y < bi.y || (ai.y == bi.y && af.y < bf.y)) {
				r.y = ai.y;
				r.yf = af.y;
			}
			else {
				r.y = bi.y;
				r.yf = bf.y;
			}
			if (ai.z < bi.z || (ai.z == bi.z && af.z < bf.z)) {
				r.z = ai.z;
				r.zf = af.z;
			}
			else {
				r.z = bi.z;
				r.zf = bf.z;
			}
			return r;
		}

		//两个Fix3在整数部分和小数部分分开保存的情况下进行比较
		DME_INLINE constexpr auto Max(const Int3& ai, const Float3& af, const Int3& bi, const Float3& bf) noexcept {
			Fix3 r = Fix3();
			if (ai.x > bi.x || (ai.x == bi.x && af.x > bf.x)) {
				r.x = ai.x;
				r.xf = af.x;
			}
			else {
				r.x = bi.x;
				r.xf = bf.x;
			}
			if (ai.y > bi.y || (ai.y == bi.y && af.y > bf.y)) {
				r.y = ai.y;
				r.yf = af.y;
			}
			else {
				r.y = bi.y;
				r.yf = bf.y;
			}
			if (ai.z > bi.z || (ai.z == bi.z && af.z > bf.z)) {
				r.z = ai.z;
				r.zf = af.z;
			}
			else {
				r.z = bi.z;
				r.zf = bf.z;
			}
			return r;
		}
	}
	using math::Fix;
	using math::FixRef;
	using math::Fix2;
	using math::Fix3;
}

namespace std {
	template<>
	struct hash<dme::math::Fix> {
		size_t operator ()(const dme::math::Fix& a) const noexcept {
			return Hasher(a);
		}
	};

	template<>
	struct hash<dme::math::FixRef> {
		size_t operator ()(const dme::math::FixRef& a) const noexcept {
			return Hasher(a);
		}
	};

	template<>
	struct hash<dme::math::Fix2> {
		size_t operator ()(const dme::math::Fix2& a) const noexcept {
			return Hasher(a);
		}
	};

	template<>
	struct hash<dme::math::Fix3> {
		size_t operator ()(const dme::math::Fix3& a) const noexcept {
			return Hasher(a);
		}
	};
}