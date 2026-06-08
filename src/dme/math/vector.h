#pragma once
#include <dme/math/utils.h>
namespace dme {
	namespace math {
		//设置为true后，通过[]操作符访问都会将参数限制在安全的范围内
		static constexpr bool _math_safe_index_ = false;

		/*
			多继承空类在MSVC下编译会导致类的大小增加（并且只有运行时才能看出来，编译前看起来大小正常），目前改为单继承模板
			如果在MSVC改为多继承，需要在子类上添加__declspec(empty_bases)，以采用正确的EBCO优化
		*/

		//向量基础模板，T为分量的类型，A为维度
		template<typename T, axis_t A>
		struct _vec {};

		//向量的常量表达式基础模板，T为分量的类型，A为维度
		template<typename T, axis_t A>
		struct _vec_constexpr {};

		//向量的根据类型独有的辅助函数模板，V为向量类型，T为分量的类型，A为维度
		template<typename V, typename T, axis_t A>
		struct _vec_type_func {};

		//向量的根据维度独有的辅助函数模板，V为向量类型，T为分量的类型，A为维度
		template<typename V, typename T, axis_t A>
		struct _vec_axis_func : public _vec_type_func<V, T, A> {};

		//bool function
		template<typename V, IsBool T, axis_t A>
		struct _vec_type_func<V, T, A> {
			constexpr T allTrue(this const V& self) noexcept {
				if constexpr (A == 2) {
					return self.x && self.y;
				}else if constexpr (A == 3) {
					return self.x && self.y && self.z;
				}else if constexpr (A == 4) {
					return self.x && self.y && self.z && self.w;
				}
			}
			constexpr T hasTrue(this const V& self) noexcept {
				if constexpr (A == 2) {
					return self.x || self.y;
				}else if constexpr (A == 3) {
					return self.x || self.y || self.z;
				}else if constexpr (A == 4) {
					return self.x || self.y || self.z || self.w;
				}
			}
			constexpr T allFalse(this const V& self) noexcept {
				if constexpr (A == 2) {
					return !(self.x || self.y);
				}else if constexpr (A == 3) {
					return !(self.x || self.y || self.z);
				}else if constexpr (A == 4) {
					return !(self.x || self.y || self.z || self.w);
				}
			}
			constexpr T hasFalse(this const V& self) noexcept {
				if constexpr (A == 2) {
					return !(self.x && self.y);
				}else if constexpr (A == 3) {
					return !(self.x && self.y && self.z);
				}else if constexpr (A == 4) {
					return !(self.x && self.y && self.z && self.w);
				}
			}
			constexpr axis_t trueCount(this const V& self) noexcept {
				if constexpr (A == 2) {
					return self.x + self.y;
				}else if constexpr (A == 3) {
					return self.x + self.y + self.z;
				}else if constexpr (A == 4) {
					return self.x + self.y + self.z + self.w;
				}
			}
			constexpr axis_t falseCount(this const V& self) noexcept {
				if constexpr (A == 2) {
					return  A - self.x - self.y;
				}else if constexpr (A == 3) {
					return  A - self.x - self.y - self.z;
				}else if constexpr (A == 4) {
					return  A - self.x - self.y - self.z - self.w;
				}
			}
		};

		//float function
		template<typename V, IsFloat T, axis_t A>
		struct _vec_type_func<V, T, A> {
			//返回向下取整，返回类型为整数
			template<typename = void>
			constexpr auto floori(this const V& self) noexcept {
				auto r = _vec<sint, A>();
				r.x = static_cast<sint>(std::floor(self.x));
				if constexpr (A > 1) {
					r.y = static_cast<sint>(std::floor(self.y));
				}
				if constexpr (A > 2) {
					r.z = static_cast<sint>(std::floor(self.z));
				}
				if constexpr (A > 3) {
					r.w = static_cast<sint>(std::floor(self.w));
				}
				return r;
			}
			//返回向上取整，返回类型为整数
			template<typename = void>
			constexpr auto ceili(this const V& self) noexcept {
				auto r = _vec<sint, A>();
				r.x = static_cast<sint>(std::ceil(self.x));
				if constexpr (A > 1) {
					r.y = static_cast<sint>(std::ceil(self.y));
				}
				if constexpr (A > 2) {
					r.z = static_cast<sint>(std::ceil(self.z));
				}
				if constexpr (A > 3) {
					r.w = static_cast<sint>(std::ceil(self.w));
				}
				return r;
			}
			//返回距离
			template<IsBaseNumber P>
			constexpr auto distance(this const V& self, const _vec<P, A>& a) noexcept {
				auto x_ = a.x - self.x;
				auto r = x_ * x_;
				if constexpr (A > 1) {
					auto y_ = a.y - self.y;
					r += y_ * y_;
				}
				if constexpr (A > 2) {
					auto z_ = a.z - self.z;
					r += z_ * z_;
				}
				if constexpr (A > 3) {
					auto w_ = a.w - self.w;
					r += w_ * w_;
				}
				return std::sqrt(r);
			}
			//返回长度
			constexpr auto length(this const V& self) noexcept {
				auto r = self.x * self.x;
				if constexpr (A > 1) {
					r += self.y * self.y;
				}
				if constexpr (A > 2) {
					r += self.z * self.z;
				}
				if constexpr (A > 3) {
					r += self.w * self.w;
				}
				return std::sqrt(r);
			}
			//返回长度的平方
			constexpr auto lengthSquare(this const V& self) noexcept {
				auto r = self.x * self.x;
				if constexpr (A > 1) {
					r += self.y * self.y;
				}
				if constexpr (A > 2) {
					r += self.z * self.z;
				}
				if constexpr (A > 3) {
					r += self.w * self.w;
				}
				return r;
			}
			//标准化
			template<typename = void>
			constexpr auto normalize(this const V& self) noexcept {
				T a = divide(T(1), self.length());
				if constexpr (A == 2) {
					return V(self.x * a, self.y * a);
				}else if constexpr (A == 3) {
					return V(self.x * a, self.y * a, self.z * a);
				}else if constexpr (A == 4) {
					return V(self.x * a, self.y * a, self.z * a, self.w * a);
				}
			}
			//倒数
			template<typename = void>
			constexpr auto inversion(this const V& self) noexcept {
				if constexpr (A == 2) {
					return V(
						divide(1, self.x),
						divide(1, self.y)
					);
				}else if constexpr (A == 3) {
					return V(
						divide(1, self.x),
						divide(1, self.y),
						divide(1, self.z)
					);
				}else if constexpr (A == 4) {
					return V(
						divide(1, self.x),
						divide(1, self.y),
						divide(1, self.z),
						divide(1, self.w)
					);
				}
			}
		};

		//2维向量的函数
		template<typename V, typename T>
		struct _vec_axis_func<V, T, 2> : public _vec_type_func<V, T, 2> {
			//叉积
			template<IsBaseNumber P>
			constexpr auto cross(this const V& self, const _vec<P, 2>& a) noexcept {
				return self.x * a.y - self.y * a.x;
			}
			//逆时针旋转90度
			constexpr auto rotateCCW90(this const V& self) noexcept {
				return V(-self.y, self.x);
			}
			//顺时针旋转90度
			constexpr auto rotateCW90(this const V& self) noexcept {
				return V(self.y, -self.x);
			}
			//中心旋转180度
			constexpr auto rotateCS180(this const V& self) noexcept {
				return V(-self.x, -self.y);
			}

			//x+=1
			constexpr auto& moveRight(this V& self) noexcept {
				self.x += 1;
				return self;
			}
			//x-=1
			constexpr auto& moveLeft(this V& self) noexcept {
				self.x -= 1;
				return self;
			}
			//y+=1，注意二维时up为y正轴，三维时up为z正轴
			constexpr auto& moveUp(this V& self) noexcept {
				self.y += 1;
				return *this;
			}
			//y-=1，注意二维时down为y负轴，三维时down为z负轴
			constexpr auto& moveDown(this V& self) noexcept {
				self.y -= 1;
				return self;
			}
			//return vec(x+1, y);
			constexpr auto right(this const V& self) noexcept {
				return V(self.x + 1, self.y);
			}
			//return vec(x-1, y);
			constexpr auto left(this const V& self) noexcept {
				return V(self.x - 1, self.y);
			}
			//return vec(x, y+1, z)，注意二维时up为y正轴，三维时up为z正轴
			constexpr auto up(this const V& self) noexcept {
				return V(self.x, self.y + 1);
			}
			//return vec(x, y-1, ...)，注意二维时down为y负轴，三维时down为z负轴
			constexpr auto down(this const V& self) noexcept {
				return V(self.x, self.y - 1);
			}

			constexpr auto xy(this const V& self) noexcept {
				return V(self.x, self.y);
			}
			constexpr auto yx(this const V& self) noexcept {
				return V(self.y, self.x);
			}
		};

		//3维向量的函数
		template<typename V, typename T>
		struct _vec_axis_func<V, T, 3> : public _vec_type_func<V, T, 3> {
			//叉积
			template<IsBaseNumber P>
			constexpr auto cross(this const V& self, const _vec<P, 3>& a) noexcept {
				return _vec<arithmetic_return_type<T, P>, 3>(
					self.y * a.z - a.y * self.z,
					self.z * a.x - a.z * self.x,
					self.x * a.y - a.x * self.y
				);
			}

			//x+=1
			constexpr auto& moveRight(this V& self) noexcept {
				self.x += 1;
				return self;
			}
			//x-=1
			constexpr auto& moveLeft(this V& self) noexcept {
				self.x -= 1;
				return self;
			}
			//z+=1，注意二维时up为y正轴，三维时up为z正轴
			constexpr auto& moveUp(this V& self) noexcept {
				self.z += 1;
				return *this;
			}
			//z-=1，注意二维时down为y负轴，三维时down为z负轴
			constexpr auto& moveDown(this V& self) noexcept {
				self.z -= 1;
				return self;
			}
			//y+=1;
			constexpr auto& moveForward(this V& self) noexcept {
				self.y += 1;
				return self;
			}
			//y-=1;
			constexpr auto& moveBackward(this V& self) noexcept {
				self.y -= 1;
				return self;
			}
			//return vec(x+1, y, z);
			constexpr auto right(this const V& self) noexcept {
				return V(self.x + 1, self.y, self.z);
			}
			//return vec(x-1, y, z);
			constexpr auto left(this const V& self) noexcept {
				return V(self.x - 1, self.y, self.z);
			}
			//return vec(x, y, z+1)，注意二维时up为y正轴，三维时up为z正轴
			constexpr auto up(this const V& self) noexcept {
				return V(self.x, self.y, self.z + 1);
			}
			//return vec(x, y, z-1)，注意二维时down为y负轴，三维时down为z负轴
			constexpr auto down(this const V& self) noexcept {
				return V(self.x, self.y, self.z - 1);
			}
			//return vec(x, y+1, z);
			constexpr auto forward(this const V& self) noexcept {
				return V(self.x, self.y + 1, self.z);
			}
			//return vec(x, y-1, z);
			constexpr auto backward(this const V& self) noexcept {
				return V(self.x, self.y - 1, self.z);
			}

			//丢弃某个轴
			constexpr auto discard(this const V& self, axis_t index) noexcept {
				if constexpr (_math_safe_index_) {
					index %= V::axis;
				}
				switch (index) {
				case 0:
					return self.vyz;
				case 1:
					return V::pre_type(self.x, self.z);
				case 2:
				default:
					return self.vxy;
				}
			}
			constexpr auto xy(this const V& self) noexcept {
				return V::pre_type(self.x, self.y);
			}
			constexpr auto xz(this const V& self) noexcept {
				return V::pre_type(self.x, self.z);
			}
			constexpr auto yz(this const V& self) noexcept {
				return V::pre_type(self.y, self.z);
			}
			constexpr auto yx(this const V& self) noexcept {
				return V::pre_type(self.y, self.x);
			}
			constexpr auto zx(this const V& self) noexcept {
				return V::pre_type(self.z, self.x);
			}
			constexpr auto zy(this const V& self) noexcept {
				return V::pre_type(self.z, self.y);
			}
			constexpr auto xyz(this const V& self) noexcept {
				return V(self.x, self.y, self.z);
			}
			constexpr auto xzy(this const V& self) noexcept {
				return V(self.x, self.z, self.y);
			}
			constexpr auto yxz(this const V& self) noexcept {
				return V(self.y, self.x, self.z);
			}
			constexpr auto yzx(this const V& self) noexcept {
				return V(self.y, self.z, self.x);
			}
			constexpr auto zxy(this const V& self) noexcept {
				return V(self.z, self.x, self.y);
			}
			constexpr auto zyx(this const V& self) noexcept {
				return V(self.z, self.y, self.x);
			}
		};

		//4维向量的函数
		template<typename V, typename T>
		struct _vec_axis_func<V, T, 4> : public _vec_type_func<V, T, 4> {
			//x+=1
			constexpr auto& moveRight(this V& self) noexcept {
				self.x += 1;
				return self;
			}
			//x-=1
			constexpr auto& moveLeft(this V& self) noexcept {
				self.x -= 1;
				return self;
			}
			//z+=1，注意二维时up为y正轴，三维时up为z正轴
			constexpr auto& moveUp(this V& self) noexcept {
				self.z += 1;
				return *this;
			}
			//z-=1，注意二维时down为y负轴，三维时down为z负轴
			constexpr auto& moveDown(this V& self) noexcept {
				self.z -= 1;
				return self;
			}
			//y+=1;
			constexpr auto& moveForward(this V& self) noexcept {
				self.y += 1;
				return self;
			}
			//y-=1;
			constexpr auto& moveBackward(this V& self) noexcept {
				self.y -= 1;
				return self;
			}
			//return vec(x+1, y, z, w);
			constexpr auto right(this const V& self) noexcept {
				return V(self.x + 1, self.y, self.z, self.w);
			}
			//return vec(x-1, y, z, w);
			constexpr auto left(this const V& self) noexcept {
				return V(self.x - 1, self.y, self.z, self.w);
			}
			//return vec(x, y, z+1, w)，注意二维时up为y正轴，三维时up为z正轴
			constexpr auto up(this const V& self) noexcept {
				return V(self.x, self.y, self.z + 1, self.w);
			}
			//return vec(x, y, z-1, w)，注意二维时down为y负轴，三维时down为z负轴
			constexpr auto down(this const V& self) noexcept {
				return V(self.x, self.y, self.z - 1, self.w);
			}
			//return vec(x, y+1, z, w);
			constexpr auto forward(this const V& self) noexcept {
				return V(self.x, self.y + 1, self.z, self.w);
			}
			//return vec(x, y-1, z, w);
			constexpr auto backward(this const V& self) noexcept {
				return V(self.x, self.y - 1, self.z, self.w);
			}

			//丢弃某个轴
			constexpr auto discard(this const V& self, axis_t index) noexcept {
				if constexpr (_math_safe_index_) {
					index %= V::axis;
				}
				switch (index) {
				case 0:
					return self.vyzw;
				case 1:
					return V::pre_type(self.x, self.vzw);
				case 2:
					return V::pre_type(self.vxy, self.w);
				case 3:
				default:
					return self.vxyz;
				}
			}
			constexpr auto xy(this const V& self) noexcept {
				return V::pre_type::pre_type(self.x, self.y);
			}
			constexpr auto xz(this const V& self) noexcept {
				return V::pre_type::pre_type(self.x, self.z);
			}
			constexpr auto xw(this const V& self) noexcept {
				return V::pre_type::pre_type(self.x, self.w);
			}
			constexpr auto yx(this const V& self) noexcept {
				return V::pre_type::pre_type(self.y, self.x);
			}
			constexpr auto yz(this const V& self) noexcept {
				return V::pre_type::pre_type(self.y, self.z);
			}
			constexpr auto yw(this const V& self) noexcept {
				return V::pre_type::pre_type(self.y, self.w);
			}
			constexpr auto zx(this const V& self) noexcept {
				return V::pre_type::pre_type(self.z, self.x);
			}
			constexpr auto zy(this const V& self) noexcept {
				return V::pre_type::pre_type(self.z, self.y);
			}
			constexpr auto zw(this const V& self) noexcept {
				return V::pre_type::pre_type(self.z, self.w);
			}
			constexpr auto wx(this const V& self) noexcept {
				return V::pre_type::pre_type(self.w, self.x);
			}
			constexpr auto wy(this const V& self) noexcept {
				return V::pre_type::pre_type(self.w, self.y);
			}
			constexpr auto wz(this const V& self) noexcept {
				return V::pre_type::pre_type(self.w, self.z);
			}
			constexpr auto xyz(this const V& self) noexcept {
				return V::pre_type(self.x, self.y, self.z);
			}
			constexpr auto xyw(this const V& self) noexcept {
				return V::pre_type(self.x, self.y, self.w);
			}
			constexpr auto xzy(this const V& self) noexcept {
				return V::pre_type(self.x, self.z, self.y);
			}
			constexpr auto xzw(this const V& self) noexcept {
				return V::pre_type(self.x, self.z, self.w);
			}
			constexpr auto xwy(this const V& self) noexcept {
				return V::pre_type(self.x, self.w, self.y);
			}
			constexpr auto xwz(this const V& self) noexcept {
				return V::pre_type(self.x, self.w, self.z);
			}
			constexpr auto yxz(this const V& self) noexcept {
				return V::pre_type(self.y, self.x, self.z);
			}
			constexpr auto yxw(this const V& self) noexcept {
				return V::pre_type(self.y, self.x, self.w);
			}
			constexpr auto yzx(this const V& self) noexcept {
				return V::pre_type(self.y, self.z, self.x);
			}
			constexpr auto yzw(this const V& self) noexcept {
				return V::pre_type(self.y, self.z, self.w);
			}
			constexpr auto ywx(this const V& self) noexcept {
				return V::pre_type(self.y, self.w, self.x);
			}
			constexpr auto ywz(this const V& self) noexcept {
				return V::pre_type(self.y, self.w, self.z);
			}
			constexpr auto zxy(this const V& self) noexcept {
				return V::pre_type(self.z, self.x, self.y);
			}
			constexpr auto zxw(this const V& self) noexcept {
				return V::pre_type(self.z, self.x, self.w);
			}
			constexpr auto zyx(this const V& self) noexcept {
				return V::pre_type(self.z, self.y, self.x);
			}
			constexpr auto zyw(this const V& self) noexcept {
				return V::pre_type(self.z, self.y, self.w);
			}
			constexpr auto zwx(this const V& self) noexcept {
				return V::pre_type(self.z, self.w, self.x);
			}
			constexpr auto zwy(this const V& self) noexcept {
				return V::pre_type(self.z, self.w, self.y);
			}
			constexpr auto wxy(this const V& self) noexcept {
				return V::pre_type(self.w, self.x, self.y);
			}
			constexpr auto wxz(this const V& self) noexcept {
				return V::pre_type(self.w, self.x, self.z);
			}
			constexpr auto wyx(this const V& self) noexcept {
				return V::pre_type(self.w, self.y, self.x);
			}
			constexpr auto wyz(this const V& self) noexcept {
				return V::pre_type(self.w, self.y, self.z);
			}
			constexpr auto wzx(this const V& self) noexcept {
				return V::pre_type(self.w, self.z, self.x);
			}
			constexpr auto wzy(this const V& self) noexcept {
				return V::pre_type(self.w, self.z, self.y);
			}
			constexpr auto xyzw(this const V& self) noexcept {
				return V(self.x, self.y, self.z, self.w);
			}
			constexpr auto xywz(this const V& self) noexcept {
				return V(self.x, self.y, self.w, self.z);
			}
			constexpr auto xzyw(this const V& self) noexcept {
				return V(self.x, self.z, self.y, self.w);
			}
			constexpr auto xzwy(this const V& self) noexcept {
				return V(self.x, self.z, self.w, self.y);
			}
			constexpr auto xwyz(this const V& self) noexcept {
				return V(self.x, self.w, self.y, self.z);
			}
			constexpr auto xwzy(this const V& self) noexcept {
				return V(self.x, self.w, self.z, self.y);
			}
			constexpr auto yxzw(this const V& self) noexcept {
				return V(self.y, self.x, self.z, self.w);
			}
			constexpr auto yxwz(this const V& self) noexcept {
				return V(self.y, self.x, self.w, self.z);
			}
			constexpr auto yzxw(this const V& self) noexcept {
				return V(self.y, self.z, self.x, self.w);
			}
			constexpr auto yzwx(this const V& self) noexcept {
				return V(self.y, self.z, self.w, self.x);
			}
			constexpr auto ywxz(this const V& self) noexcept {
				return V(self.y, self.w, self.x, self.z);
			}
			constexpr auto ywzx(this const V& self) noexcept {
				return V(self.y, self.w, self.x, self.z);
			}
			constexpr auto zxyw(this const V& self) noexcept {
				return V(self.z, self.x, self.y, self.w);
			}
			constexpr auto zxwy(this const V& self) noexcept {
				return V(self.z, self.x, self.w, self.y);
			}
			constexpr auto zyxw(this const V& self) noexcept {
				return V(self.z, self.y, self.x, self.w);
			}
			constexpr auto zywx(this const V& self) noexcept {
				return V(self.z, self.y, self.w, self.x);
			}
			constexpr auto zwxy(this const V& self) noexcept {
				return V(self.z, self.w, self.x, self.w);
			}
			constexpr auto zwyx(this const V& self) noexcept {
				return V(self.z, self.w, self.y, self.x);
			}
			constexpr auto wxyz(this const V& self) noexcept {
				return V(self.w, self.x, self.y, self.z);
			}
			constexpr auto wxzy(this const V& self) noexcept {
				return V(self.w, self.x, self.z, self.y);
			}
			constexpr auto wyxz(this const V& self) noexcept {
				return V(self.w, self.y, self.x, self.z);
			}
			constexpr auto wyzx(this const V& self) noexcept {
				return V(self.w, self.y, self.z, self.x);
			}
			constexpr auto wzxy(this const V& self) noexcept {
				return V(self.w, self.z, self.x, self.y);
			}
			constexpr auto wzyx(this const V& self) noexcept {
				return V(self.w, self.z, self.y, self.x);
			}
		};

		//向量的通用辅助函数模板，V为向量类型，T为分量的类型，A为维度
		template<typename V, typename T, axis_t A>
		struct _vec_func : public _vec_axis_func<V, T, A> {
			constexpr bool isZero(this const V& self) noexcept {
				if constexpr (A == 2) {
					return self.x == T(0) && self.y == T(0);
				}else if constexpr (A == 3) {
					return self.x == T(0) && self.y == T(0) && self.z == T(0);
				}else if constexpr (A == 4) {
					return self.x == T(0) && self.y == T(0) && self.z == T(0) && self.w == T(0);
				}
			}
			template<IsBaseNumber P, axis_t AXIS = A> requires (AXIS > 1 && AXIS <= 4)
			constexpr auto cast(this const V& self) noexcept {
				if constexpr (AXIS == 2) {
					return _vec<P, AXIS>(
						static_cast<P>(self.x),
						static_cast<P>(self.y)
					);
				}else if constexpr (AXIS == 3) {
					if constexpr (A == 2) {
						return _vec<P, AXIS>(
							static_cast<P>(self.x),
							static_cast<P>(self.y),
							static_cast<P>(0)
						);
					}
					else {
						return _vec<P, AXIS>(
							static_cast<P>(self.x),
							static_cast<P>(self.y),
							static_cast<P>(self.z)
						);
					}
				}else if constexpr (AXIS == 4) {
					if constexpr (A == 2) {
						return _vec<P, AXIS>(
							static_cast<P>(self.x),
							static_cast<P>(self.y),
							static_cast<P>(0),
							static_cast<P>(0)
						);
					}else if constexpr (A == 3) {
						return _vec<P, AXIS>(
							static_cast<P>(self.x),
							static_cast<P>(self.y),
							static_cast<P>(self.z),
							static_cast<P>(0)
						);
					}
					else {
						return _vec<P, AXIS>(
							static_cast<P>(self.x),
							static_cast<P>(self.y),
							static_cast<P>(self.z),
							static_cast<P>(self.w)
						);
					}
				}
			}
			//类似glsl的swizzling语法，返回一个对应轴的新向量
			template<IsBaseNumber P = T>
			constexpr auto get(this const V& self, axis_t axis0, axis_t axis1) noexcept {
				if constexpr (_math_safe_index_) {
					axis0 %= A;
					axis1 %= A;
				}
				return _vec<P, 2>(
					self.v[axis0],
					self.v[axis1]
				);
			}
			//类似glsl的swizzling语法，返回一个对应轴的新向量
			template<IsBaseNumber P = T>
			constexpr auto get(this const V& self, axis_t axis0, axis_t axis1, axis_t axis2) noexcept {
				if constexpr (_math_safe_index_) {
					axis0 %= A;
					axis1 %= A;
					axis2 %= A;
				}
				return _vec<P, 3>(
					self.v[axis0],
					self.v[axis1],
					self.v[axis2]
				);
			}
			//类似glsl的swizzling语法，返回一个对应轴的新向量
			template<IsBaseNumber P = T>
			constexpr auto get(this const V& self, axis_t axis0, axis_t axis1, axis_t axis2, axis_t axis3) noexcept {
				if constexpr (_math_safe_index_) {
					axis0 %= A;
					axis1 %= A;
					axis2 %= A;
					axis3 %= A;
				}
				return _vec<P, 4>(
					self.v[axis0],
					self.v[axis1],
					self.v[axis2],
					self.v[axis3]
				);
			}
			//返回将当前向量每个分量的长度都化为1的向量，0除外
			template<typename = void>
			constexpr auto toDirection(this const V& self) noexcept {
				auto r = _vec<axis_st, A>();
				if (self.x > 0) { r.x = 1; }
				else if (self.x < 0) { r.x = -1; }
				if constexpr (A > 1) {
					if (self.y > 0) { r.y = 1; }
					else if (self.y < 0) { r.y = -1; }
				}
				if constexpr (A > 2) {
					if (self.z > 0) { r.z = 1; }
					else if (self.z < 0) { r.z = -1; }
				}
				if constexpr (A > 3) {
					if (self.w > 0) { r.w = 1; }
					else if (self.w < 0) { r.w = -1; }
				}
				return r;
			}
			//返回当前向量每个分量的符号，正为1负为-1
			template<typename = void>
			constexpr auto toSign(this const V& self) noexcept {
				auto r = _vec<axis_st, A>();
				r.x = self.x < 0 ? -1 : 1;
				if constexpr (A > 1) {
					r.y = self.y < 0 ? -1 : 1;
				}
				if constexpr (A > 2) {
					r.z = self.z < 0 ? -1 : 1;
				}
				if constexpr (A > 3) {
					r.w = self.w < 0 ? -1 : 1;
				}
				return r;
			}
			//返回曼哈顿距离
			template<IsBaseNumber P>
			constexpr auto manhattanDistance(this const V& self, const _vec<P, A>& a) noexcept {
				auto r = Diff(self.x, a.x);
				if constexpr (A > 1) {
					r += Diff(self.y, a.y);
				}
				if constexpr (A > 2) {
					r += Diff(self.z, a.z);
				}
				if constexpr (A > 3) {
					r += Diff(self.w, a.w);
				}
				return r;
			}
			//返回绝对差
			template<IsBaseNumber P>
			constexpr auto diff(this const V& self, const _vec<P, A>& a) noexcept {
				auto r = _vec<arithmetic_return_type<T, P>, A>();
				r.x = Diff(self.x, a.x);
				if constexpr (A > 1) {
					r.y = Diff(self.y, a.y);
				}
				if constexpr (A > 2) {
					r.z = Diff(self.z, a.z);
				}
				if constexpr (A > 3) {
					r.w = Diff(self.w, a.w);
				}
				return r;
			}
			//点积
			template<IsBaseNumber P>
			constexpr auto dot(this const V& self, const _vec<P, A>& a) noexcept {
				auto r = self.x * a.x;
				if constexpr (A > 1) {
					r += self.y * a.y;
				}
				if constexpr (A > 2) {
					r += self.z * a.z;
				}
				if constexpr (A > 3) {
					r += self.w * a.w;
				}
				return r;
			}

			//返回xyzw按绝对值的排序从大到小，优先度w>=z>=y>=x
			template<typename = void>
			constexpr auto sortAbsDescIndex(this const V& self) noexcept {
				auto a = Abs(self);
				if constexpr (A == 2) {
					if (a.x > a.y) {
						return _vec<axis_t, A>(0, 1);
					}
					else {
						return _vec<axis_t, A>(1, 0);
					}
				}
				else if constexpr (A == 3) {
					if (a.x > a.y) {
						if (a.y > a.z) {
							return _vec<axis_t, A>(0, 1, 2);
						}
						else if (a.x > a.z) {
							return _vec<axis_t, A>(0, 2, 1);
						}
						else {
							return _vec<axis_t, A>(2, 0, 1);
						}
					}
					else {
						if (a.x > a.z) {
							return _vec<axis_t, A>(1, 0, 2);
						}
						else if (a.y > a.z) {
							return _vec<axis_t, A>(1, 2, 0);
						}
						else {
							return _vec<axis_t, A>(2, 1, 0);
						}
					}
				}
				else if constexpr (A == 4) {
					if (a.x > a.y) {
						if (a.y > a.z) {
							if (a.z > a.w) {
								return _vec<axis_t, A>(0, 1, 2, 3);
							}
							else if (a.y > a.w) {
								return _vec<axis_t, A>(0, 1, 3, 2);
							}
							else if (a.x > a.w) {
								return _vec<axis_t, A>(0, 2, 3, 1);
							}
							else {
								return _vec<axis_t, A>(1, 2, 3, 0);
							}
						}
						else if (a.x > a.z) {
							if (a.y > a.w) {
								return _vec<axis_t, A>(0, 2, 1, 3);
							}
							else if (a.z > a.w) {
								return _vec<axis_t, A>(0, 3, 1, 2);
							}
							else if (a.x > a.w) {
								return _vec<axis_t, A>(0, 3, 2, 1);
							}
							else {
								return _vec<axis_t, A>(1, 3, 2, 0);
							}
						}
						else {
							if (a.x > a.w) {
								return _vec<axis_t, A>(2, 0, 1, 3);
							}
							else if (a.z > a.w) {
								return _vec<axis_t, A>(3, 0, 1, 2);
							}
							else if (a.y > a.w) {
								return _vec<axis_t, A>(3, 0, 2, 1);
							}
							else {
								return _vec<axis_t, A>(3, 1, 2, 0);
							}
						}
					}
					else {
						if (a.x > a.z) {
							if (a.z > a.w) {
								return _vec<axis_t, A>(1, 0, 2, 3);
							}
							else if (a.x > a.w) {
								return _vec<axis_t, A>(1, 0, 3, 2);
							}
							else if (a.y > a.w) {
								return _vec<axis_t, A>(2, 0, 3, 1);
							}
							else {
								return _vec<axis_t, A>(2, 1, 3, 0);
							}
						}
						else if (a.y > a.z) {
							if (a.y > a.w) {
								return _vec<axis_t, A>(1, 2, 0, 3);
							}
							else if (a.x > a.w) {
								return _vec<axis_t, A>(1, 3, 0, 2);
							}
							else if (a.z > a.w) {
								return _vec<axis_t, A>(2, 3, 0, 1);
							}
							else {
								return _vec<axis_t, A>(2, 3, 1, 0);
							}
						}
						else {
							if (a.x > a.w) {
								return _vec<axis_t, A>(2, 1, 0, 3);
							}
							else if (a.y > a.w) {
								return _vec<axis_t, A>(3, 1, 0, 2);
							}
							else if (a.z > a.w) {
								return _vec<axis_t, A>(3, 2, 0, 1);
							}
							else {
								return _vec<axis_t, A>(3, 2, 1, 0);
							}
						}
					}
				}
			}
		};

		//vec2
		template<IsBaseNumber T>
		struct _vec<T, 2> : public _vec_func<_vec<T, 2>, T, 2>
		{
			static constexpr axis_t axis = 2;
			using type = typename T;
			union {
				struct {
					T x; T y;
				};
				T v[axis];
			};
			constexpr _vec() noexcept : x(0), y(0) {}
			constexpr explicit _vec(T x) noexcept : x(x), y(x) {}
			constexpr _vec(T x, T y) noexcept : x(x), y(y) {}

			friend std::ostream& operator<<(std::ostream& os, const _vec& a) {
				return os << "x: " << a.x << ", y: " << a.y;
			}
			friend constexpr Hash Hasher(const _vec& a) noexcept {
				return Hash::Combine(a.x, a.y);
			}
			constexpr auto operator -() const noexcept {
				if constexpr (is_safe_negative_sign<T>) {
					return _vec(-x, -y);
				}
				else if constexpr (is_bool<T>) {
					return _vec(!x, !y);
				}
				else if constexpr (is_unsigned<T>) {
					return _vec(
						negativeUnsiged(x),
						negativeUnsiged(y)
					);
				}
				else {
					return _vec(x, y);
				}
			}
			constexpr T& operator [](axis_t a) noexcept {
				if constexpr (_math_safe_index_) {
					a %= axis;
				}
				return v[a];
			}
			constexpr const T& operator [](axis_t a) const noexcept {
				if constexpr (_math_safe_index_) {
					a %= axis;
				}
				return v[a];
			}
			template<IsBaseNumber P>
			constexpr bool operator ==(const _vec<P, axis>& a) const noexcept {
				return x == a.x && y == a.y;
			}
			template<IsBaseNumber P>
			constexpr bool operator !=(const _vec<P, axis>& a) const noexcept {
				return x != a.x || y != a.y;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator =(P a) noexcept {
				x = static_cast<T>(a);
				y = static_cast<T>(a);
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator =(const _vec<P, axis>& a) noexcept {
				x = static_cast<T>(a.x);
				y = static_cast<T>(a.y);
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator +=(P a) noexcept {
				x += static_cast<T>(a);
				y += static_cast<T>(a);
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator +=(const _vec<P, axis>& a) noexcept {
				x += static_cast<T>(a.x);
				y += static_cast<T>(a.y);
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto operator +(P a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x + a,
					y + a
				);
			}
			template<IsBaseNumber P>
			constexpr auto operator +(const _vec<P, axis>& a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x + a.x,
					y + a.y
				);
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator -=(P a) noexcept {
				x -= static_cast<T>(a);
				y -= static_cast<T>(a);
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator -=(const _vec<P, axis>& a) noexcept {
				x -= static_cast<T>(a.x);
				y -= static_cast<T>(a.y);
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto operator -(P a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x - a,
					y - a
				);
			}
			template<IsBaseNumber P>
			constexpr auto operator -(const _vec<P, axis>& a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x - a.x,
					y - a.y
				);
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator *=(P a) noexcept {
				x *= static_cast<T>(a);
				y *= static_cast<T>(a);
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator *=(const _vec<P, axis>& a) noexcept {
				x *= static_cast<T>(a.x);
				y *= static_cast<T>(a.y);
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto operator *(P a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x * a,
					y * a
				);
			}
			template<IsBaseNumber P>
			constexpr auto operator *(const _vec<P, axis>& a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x * a.x,
					y * a.y
				);
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator /=(P a) noexcept {
				x = math::divide(x, static_cast<T>(a));
				y = math::divide(y, static_cast<T>(a));
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator /=(const _vec<P, axis>& a) noexcept {
				x = math::divide(x, static_cast<T>(a.x));
				y = math::divide(y, static_cast<T>(a.y));
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto operator /(P a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					math::divide(x, a),
					math::divide(y, a)
				);
			}
			template<IsBaseNumber P>
			constexpr auto operator /(const _vec<P, axis>& a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					math::divide(x, a.x),
					math::divide(y, a.y)
				);
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator %=(P a) noexcept {
				x = math::modulo(x, static_cast<T>(a));
				y = math::modulo(y, static_cast<T>(a));
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator %=(const _vec<P, axis>& a) noexcept {
				x = math::modulo(x, static_cast<T>(a.x));
				y = math::modulo(y, static_cast<T>(a.y));
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto operator %(P a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					math::modulo(x, a),
					math::modulo(y, a)
				);
			}
			template<IsBaseNumber P>
			constexpr auto operator %(const _vec<P, axis>& a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					math::modulo(x, a.x),
					math::modulo(y, a.y)
				);
			}
		};

		//vec3
		template<IsBaseNumber T>
		struct _vec<T, 3> : public _vec_func<_vec<T, 3>, T, 3>
		{
			static constexpr axis_t axis = 3;
			using type = typename T;
			using pre_type = typename _vec<T, axis - 1>;
			union {
				struct {
					T x; T y; T z;
				};
				struct {
					_vec<T, axis - 1> vxy;
					T _vxy_z;
				};
				struct {
					T _vyz_x;
					_vec<T, axis - 1> vyz;
				};
				T v[axis];
			};
			constexpr _vec() noexcept : x(0), y(0), z(0) {}
			constexpr explicit _vec(T x) noexcept : x(x), y(x), z(x) {}
			constexpr _vec(T x, T y, T z) noexcept : x(x), y(y), z(z) {}

			constexpr _vec(_vec<T, axis - 1> vxy, T z) noexcept : vxy(vxy), _vxy_z(z) {}
			constexpr _vec(T x, _vec<T, axis - 1> vyz) noexcept : _vyz_x(x), vyz(vyz) {}

			friend std::ostream& operator<<(std::ostream& os, const _vec& a) {
				return os << "x: " << a.x << ", y: " << a.y << ", z: " << a.z;
			}
			friend constexpr Hash Hasher(const _vec& a) noexcept {
				return Hash::Combine(a.x, a.y, a.z);
			}
			constexpr auto operator -() const noexcept {
				if constexpr (is_safe_negative_sign<T>) {
					return _vec(-x, -y, -z);
				}
				else if constexpr (is_bool<T>) {
					return _vec(!x, !y, !z);
				}
				else if constexpr (is_unsigned<T>) {
					return _vec(
						negativeUnsiged(x),
						negativeUnsiged(y),
						negativeUnsiged(z)
					);
				}
				else {
					return _vec(x, y, z);
				}
			}
			constexpr T& operator [](axis_t a) noexcept {
				if constexpr (_math_safe_index_) {
					a %= axis;
				}
				return v[a];
			}
			constexpr const T& operator [](axis_t a) const noexcept {
				if constexpr (_math_safe_index_) {
					a %= axis;
				}
				return v[a];
			}
			template<IsBaseNumber P>
			constexpr bool operator ==(const _vec<P, axis>& a) const noexcept {
				return x == a.x && y == a.y && z == a.z;
			}
			template<IsBaseNumber P>
			constexpr bool operator !=(const _vec<P, axis>& a) const noexcept {
				return x != a.x || y != a.y || z != a.z;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator =(P a) noexcept {
				x = static_cast<T>(a);
				y = static_cast<T>(a);
				z = static_cast<T>(a);
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator =(const _vec<P, axis>& a) noexcept {
				x = static_cast<T>(a.x);
				y = static_cast<T>(a.y);
				z = static_cast<T>(a.z);
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator +=(P a) noexcept {
				x += static_cast<T>(a);
				y += static_cast<T>(a);
				z += static_cast<T>(a);
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator +=(const _vec<P, axis>& a) noexcept {
				x += static_cast<T>(a.x);
				y += static_cast<T>(a.y);
				z += static_cast<T>(a.z);
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto operator +(P a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x + a,
					y + a,
					z + a
				);
			}
			template<IsBaseNumber P>
			constexpr auto operator +(const _vec<P, axis>& a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x + a.x,
					y + a.y,
					z + a.z
				);
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator -=(P a) noexcept {
				x -= static_cast<T>(a);
				y -= static_cast<T>(a);
				z -= static_cast<T>(a);
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator -=(const _vec<P, axis>& a) noexcept {
				x -= static_cast<T>(a.x);
				y -= static_cast<T>(a.y);
				z -= static_cast<T>(a.z);
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto operator -(P a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x - a,
					y - a,
					z - a
				);
			}
			template<IsBaseNumber P>
			constexpr auto operator -(const _vec<P, axis>& a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x - a.x,
					y - a.y,
					z - a.z
				);
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator *=(P a) noexcept {
				x *= static_cast<T>(a);
				y *= static_cast<T>(a);
				z *= static_cast<T>(a);
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator *=(const _vec<P, axis>& a) noexcept {
				x *= static_cast<T>(a.x);
				y *= static_cast<T>(a.y);
				z *= static_cast<T>(a.z);
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto operator *(P a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x * a,
					y * a,
					z * a
				);
			}
			template<IsBaseNumber P>
			constexpr auto operator *(const _vec<P, axis>& a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x * a.x,
					y * a.y,
					z * a.z
				);
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator /=(P a) noexcept {
				x = math::divide(x, static_cast<T>(a));
				y = math::divide(y, static_cast<T>(a));
				z = math::divide(z, static_cast<T>(a));
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator /=(const _vec<P, axis>& a) noexcept {
				x = math::divide(x, static_cast<T>(a.x));
				y = math::divide(y, static_cast<T>(a.y));
				z = math::divide(z, static_cast<T>(a.z));
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto operator /(P a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					math::divide(x, a),
					math::divide(y, a),
					math::divide(z, a)
				);
			}
			template<IsBaseNumber P>
			constexpr auto operator /(const _vec<P, axis>& a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					math::divide(x, a.x),
					math::divide(y, a.y),
					math::divide(z, a.z)
				);
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator %=(P a) noexcept {
				x = math::modulo(x, static_cast<T>(a));
				y = math::modulo(y, static_cast<T>(a));
				z = math::modulo(z, static_cast<T>(a));
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator %=(const _vec<P, axis>& a) noexcept {
				x = math::modulo(x, static_cast<T>(a.x));
				y = math::modulo(y, static_cast<T>(a.y));
				z = math::modulo(z, static_cast<T>(a.z));
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto operator %(P a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					math::modulo(x, a),
					math::modulo(y, a),
					math::modulo(z, a)
				);
			}
			template<IsBaseNumber P>
			constexpr auto operator %(const _vec<P, axis>& a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					math::modulo(x, a.x),
					math::modulo(y, a.y),
					math::modulo(z, a.z)
				);
			}
		};

		//vec4
		template<IsBaseNumber T>
		struct _vec<T, 4> : public _vec_func<_vec<T, 4>, T, 4>
		{
			static constexpr axis_t axis = 4;
			using type = typename T;
			using pre_type = typename _vec<T, axis - 1>;
			union {
				struct {
					T x; T y; T z; T w;
				};
				struct {
					_vec<T, axis - 2> vxy;
					_vec<T, axis - 2> vzw;
				};
				struct {
					T _vyz_x;
					_vec<T, axis - 2> vyz;
					T _vyz_w;
				};
				_vec<T, axis - 2> v2[2];
				struct {
					_vec<T, axis - 1> vxyz;
					T _vxyz_w;
				};
				struct {
					T _vyzw_x;
					_vec<T, axis - 1> vyzw;
				};
				T v[axis];
			};
			constexpr _vec() noexcept : x(0), y(0), z(0), w(0) {}
			constexpr explicit _vec(T x) noexcept : x(x), y(x), z(x), w(x) {}
			constexpr _vec(T x, T y, T z, T w) noexcept : x(x), y(y), z(z), w(w) {}

			constexpr _vec(_vec<T, axis - 2> vxy, T z, T w) noexcept : vxy(vxy), vzw(z, w) {}
			constexpr _vec(T x, T y, _vec<T, axis - 2> vzw) noexcept : vxy(x, y), vzw(vzw) {}

			constexpr _vec(_vec<T, axis - 2> vxy, _vec<T, axis - 2> vzw) noexcept : vxy(vxy), vzw(vzw) {}

			constexpr _vec(T x, const _vec<T, axis - 2>& vyz, T w) noexcept : _vyz_x(x), vyz(vyz), _vyz_w(w) {}
			constexpr _vec(T x, _vec<T, axis - 2>&& vyz, T w) noexcept : _vyz_x(x), vyz(vyz), _vyz_w(w) {}
			constexpr _vec(const _vec<T, axis - 1>& vxyz, T w) noexcept : vxyz(vxyz), _vxyz_w(w) {}
			constexpr _vec(_vec<T, axis - 1>&& vxyz, T w) noexcept : vxyz(vxyz), _vxyz_w(w) {}
			constexpr _vec(T x, const _vec<T, axis - 1>& vyzw) noexcept : _vyzw_x(x), vyzw(vyzw) {}
			constexpr _vec(T x, _vec<T, axis - 1>&& vyzw) noexcept : _vyzw_x(x), vyzw(vyzw) {}

			friend std::ostream& operator<<(std::ostream& os, const _vec& a) {
				return os << "x: " << a.x << ", y: " << a.y << ", z: " << a.z << ", w: " << a.w;
			}
			friend constexpr Hash Hasher(const _vec& a) noexcept {
				return Hash::Combine(a.x, a.y, a.z, a.w);
			}
			constexpr auto operator -() const noexcept {
				if constexpr (is_safe_negative_sign<T>) {
					return _vec(-x, -y, -z, -w);
				}
				else if constexpr (is_bool<T>) {
					return _vec(!x, !y, !z, !w);
				}
				else if constexpr (is_unsigned<T>) {
					return _vec(
						negativeUnsiged(x),
						negativeUnsiged(y),
						negativeUnsiged(z),
						negativeUnsiged(w)
					);
				}
				else {
					return _vec(x, y, z, w);
				}
			}
			constexpr T& operator [](axis_t a) noexcept {
				if constexpr (_math_safe_index_) {
					a %= axis;
				}
				return v[a];
			}
			constexpr const T& operator [](axis_t a) const noexcept {
				if constexpr (_math_safe_index_) {
					a %= axis;
				}
				return v[a];
			}

			template<IsBaseNumber P>
			constexpr bool operator ==(const _vec<P, axis>& a) const noexcept {
				return x == a.x && y == a.y && z == a.z && w == a.w;
			}
			template<IsBaseNumber P>
			constexpr bool operator !=(const _vec<P, axis>& a) const noexcept {
				return x != a.x || y != a.y || z != a.z || w != a.w;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator =(P a) noexcept {
				x = static_cast<T>(a);
				y = static_cast<T>(a);
				z = static_cast<T>(a);
				w = static_cast<T>(a);
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator =(const _vec<P, axis>& a) noexcept {
				x = static_cast<T>(a.x);
				y = static_cast<T>(a.y);
				z = static_cast<T>(a.z);
				w = static_cast<T>(a.w);
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator +=(P a) noexcept {
				x += static_cast<T>(a);
				y += static_cast<T>(a);
				z += static_cast<T>(a);
				w += static_cast<T>(a);
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator +=(const _vec<P, axis>& a) noexcept {
				x += static_cast<T>(a.x);
				y += static_cast<T>(a.y);
				z += static_cast<T>(a.z);
				w += static_cast<T>(a.w);
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto operator +(P a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x + a,
					y + a,
					z + a,
					w + a
				);
			}
			template<IsBaseNumber P>
			constexpr auto operator +(const _vec<P, axis>& a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x + a.x,
					y + a.y,
					z + a.z,
					w + a.w
				);
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator -=(P a) noexcept {
				x -= static_cast<T>(a);
				y -= static_cast<T>(a);
				z -= static_cast<T>(a);
				w -= static_cast<T>(a);
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator -=(const _vec<P, axis>& a) noexcept {
				x -= static_cast<T>(a.x);
				y -= static_cast<T>(a.y);
				z -= static_cast<T>(a.z);
				w -= static_cast<T>(a.w);
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto operator -(P a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x - a,
					y - a,
					z - a,
					w - a
				);
			}
			template<IsBaseNumber P>
			constexpr auto operator -(const _vec<P, axis>& a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x - a.x,
					y - a.y,
					z - a.z,
					w - a.w
				);
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator *=(P a) noexcept {
				x *= static_cast<T>(a);
				y *= static_cast<T>(a);
				z *= static_cast<T>(a);
				w *= static_cast<T>(a);
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator *=(const _vec<P, axis>& a) noexcept {
				x *= static_cast<T>(a.x);
				y *= static_cast<T>(a.y);
				z *= static_cast<T>(a.z);
				w *= static_cast<T>(a.w);
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto operator *(P a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x * a,
					y * a,
					z * a,
					w * a
				);
			}
			template<IsBaseNumber P>
			constexpr auto operator *(const _vec<P, axis>& a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					x * a.x,
					y * a.y,
					z * a.z,
					w * a.w
				);
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator /=(P a) noexcept {
				x = math::divide(x, static_cast<T>(a));
				y = math::divide(y, static_cast<T>(a));
				z = math::divide(z, static_cast<T>(a));
				w = math::divide(w, static_cast<T>(a));
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator /=(const _vec<P, axis>& a) noexcept {
				x = math::divide(x, static_cast<T>(a.x));
				y = math::divide(y, static_cast<T>(a.y));
				z = math::divide(z, static_cast<T>(a.z));
				w = math::divide(w, static_cast<T>(a.w));
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto operator /(P a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					math::divide(x, a),
					math::divide(y, a),
					math::divide(z, a),
					math::divide(w, a)
				);
			}
			template<IsBaseNumber P>
			constexpr auto operator /(const _vec<P, axis>& a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					math::divide(x, a.x),
					math::divide(y, a.y),
					math::divide(z, a.z),
					math::divide(w, a.w)
				);
			}
			template<IsBaseNumberAssignment P>
			constexpr auto& operator %=(P a) noexcept {
				x = math::modulo(x, static_cast<T>(a));
				y = math::modulo(y, static_cast<T>(a));
				z = math::modulo(z, static_cast<T>(a));
				w = math::modulo(w, static_cast<T>(a));
				return *this;
			}
			template<IsBaseNumber P>
			constexpr auto& operator %=(const _vec<P, axis>& a) noexcept {
				x = math::modulo(x, static_cast<T>(a.x));
				y = math::modulo(y, static_cast<T>(a.y));
				z = math::modulo(z, static_cast<T>(a.z));
				w = math::modulo(w, static_cast<T>(a.w));
				return *this;
			}
			template<IsBaseNumberAssignment P>
			constexpr auto operator %(P a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					math::modulo(x, a),
					math::modulo(y, a),
					math::modulo(z, a),
					math::modulo(w, a)
				);
			}
			template<IsBaseNumber P>
			constexpr auto operator %(const _vec<P, axis>& a) const noexcept {
				return _vec<arithmetic_return_type<T, P>, axis>(
					math::modulo(x, a.x),
					math::modulo(y, a.y),
					math::modulo(z, a.z),
					math::modulo(w, a.w)
				);
			}
		};

		//处理非有符号类型的-1
		template<typename T>
		static constexpr auto _handle_utype_(sint v){
			if constexpr (is_bool<T>) {
				return v != 0;
			}
			else {
				return static_cast<T>(v);
			}
		}

		template<typename T>
		struct _vec_constexpr<T, 2> {
			static constexpr auto Right =		_vec<T, 2>(_handle_utype_<T>( 1), _handle_utype_<T>( 0));
			static constexpr auto Left =		_vec<T, 2>(_handle_utype_<T>(-1), _handle_utype_<T>( 0));
			static constexpr auto Up =			_vec<T, 2>(_handle_utype_<T>( 0), _handle_utype_<T>( 1));
			static constexpr auto Down =		_vec<T, 2>(_handle_utype_<T>( 0), _handle_utype_<T>(-1));
			static constexpr auto UpperRight =	_vec<T, 2>(_handle_utype_<T>( 1), _handle_utype_<T>( 1));
			static constexpr auto UpperLeft =	_vec<T, 2>(_handle_utype_<T>(-1), _handle_utype_<T>( 1));
			static constexpr auto LowerRight =	_vec<T, 2>(_handle_utype_<T>( 1), _handle_utype_<T>(-1));
			static constexpr auto LowerLeft =	_vec<T, 2>(_handle_utype_<T>(-1), _handle_utype_<T>(-1));
		};
		template<typename T>
		struct _vec_constexpr<T, 3> {
			static constexpr auto Right =		_vec<T, 3>(_handle_utype_<T>( 1), _handle_utype_<T>( 0), _handle_utype_<T>( 0));
			static constexpr auto Left =		_vec<T, 3>(_handle_utype_<T>(-1), _handle_utype_<T>( 0), _handle_utype_<T>( 0));
			static constexpr auto Forward =		_vec<T, 3>(_handle_utype_<T>( 0), _handle_utype_<T>( 1), _handle_utype_<T>( 0));
			static constexpr auto Backward =	_vec<T, 3>(_handle_utype_<T>( 0), _handle_utype_<T>(-1), _handle_utype_<T>( 0));
			static constexpr auto Up =			_vec<T, 3>(_handle_utype_<T>( 0), _handle_utype_<T>( 0), _handle_utype_<T>( 1));
			static constexpr auto Down =		_vec<T, 3>(_handle_utype_<T>( 0), _handle_utype_<T>( 0), _handle_utype_<T>(-1));
		};
		template<typename T>
		struct _vec_constexpr<T, 4> {
			static constexpr auto Right =		_vec<T, 4>(_handle_utype_<T>( 1), _handle_utype_<T>( 0), _handle_utype_<T>( 0), _handle_utype_<T>( 0));
			static constexpr auto Left =		_vec<T, 4>(_handle_utype_<T>(-1), _handle_utype_<T>( 0), _handle_utype_<T>( 0), _handle_utype_<T>( 0));
			static constexpr auto Forward =		_vec<T, 4>(_handle_utype_<T>( 0), _handle_utype_<T>( 1), _handle_utype_<T>( 0), _handle_utype_<T>( 0));
			static constexpr auto Backward =	_vec<T, 4>(_handle_utype_<T>( 0), _handle_utype_<T>(-1), _handle_utype_<T>( 0), _handle_utype_<T>( 0));
			static constexpr auto Up =			_vec<T, 4>(_handle_utype_<T>( 0), _handle_utype_<T>( 0), _handle_utype_<T>( 1), _handle_utype_<T>( 0));
			static constexpr auto Down =		_vec<T, 4>(_handle_utype_<T>( 0), _handle_utype_<T>( 0), _handle_utype_<T>(-1), _handle_utype_<T>( 0));
		};

		using Uchar2 = typename _vec<uchar, 2>;
		using Uchar2c = typename _vec_constexpr<uchar, 2>;
		using Char2 = typename _vec<schar, 2>;
		using Char2c = typename _vec_constexpr<schar, 2>;
		using Ushort2 = typename _vec<ushort, 2>;
		using Ushort2c = typename _vec_constexpr<ushort, 2>;
		using Short2 = typename _vec<sshort, 2>;
		using Short2c = typename _vec_constexpr<sshort, 2>;
		using Uint2 = typename _vec<uint, 2>;
		using Uint2c = typename _vec_constexpr<uint, 2>;
		using Int2 = typename _vec<sint, 2>;
		using Int2c = typename _vec_constexpr<sint, 2>;
		using Ulong2 = typename _vec<ulong, 2>;
		using Ulong2c = typename _vec_constexpr<ulong, 2>;
		using Long2 = typename _vec<slong, 2>;
		using Long2c = typename _vec_constexpr<slong, 2>;
		using Float2 = typename _vec<float, 2>;
		using Float2c = typename _vec_constexpr<float, 2>;
		using Double2 = typename _vec<double, 2>;
		using Double2c = typename _vec_constexpr<double, 2>;
		using Bool2 = typename _vec<bool, 2>;
		using Bool2c = typename _vec_constexpr<bool, 2>;

		using Uchar3 = typename _vec<uchar, 3>;
		using Uchar3c = typename _vec_constexpr<uchar, 3>;
		using Char3 = typename _vec<schar, 3>;
		using Char3c = typename _vec_constexpr<schar, 3>;
		using Ushort3 = typename _vec<ushort, 3>;
		using Ushort3c = typename _vec_constexpr<ushort, 3>;
		using Short3 = typename _vec<sshort, 3>;
		using Short3c = typename _vec_constexpr<sshort, 3>;
		using Uint3 = typename _vec<uint, 3>;
		using Uint3c = typename _vec_constexpr<uint, 3>;
		using Int3 = typename _vec<sint, 3>;
		using Int3c = typename _vec_constexpr<sint, 3>;
		using Ulong3 = typename _vec<ulong, 3>;
		using Ulong3c = typename _vec_constexpr<ulong, 3>;
		using Long3 = typename _vec<slong, 3>;
		using Long3c = typename _vec_constexpr<slong, 3>;
		using Float3 = typename _vec<float, 3>;
		using Float3c = typename _vec_constexpr<float, 3>;
		using Double3 = typename _vec<double, 3>;
		using Double3c = typename _vec_constexpr<double, 3>;
		using Bool3 = typename _vec<bool, 3>;
		using Bool3c = typename _vec_constexpr<bool, 3>;

		using Uchar4 = typename _vec<uchar, 4>;
		using Uchar4c = typename _vec_constexpr<uchar, 4>;
		using Char4 = typename _vec<schar, 4>;
		using Char4c = typename _vec_constexpr<schar, 4>;
		using Ushort4 = typename _vec<ushort, 4>;
		using Ushort4c = typename _vec_constexpr<ushort, 4>;
		using Short4 = typename _vec<sshort, 4>;
		using Short4c = typename _vec_constexpr<sshort, 4>;
		using Uint4 = typename _vec<uint, 4>;
		using Uint4c = typename _vec_constexpr<uint, 4>;
		using Int4 = typename _vec<sint, 4>;
		using Int4c = typename _vec_constexpr<sint, 4>;
		using Ulong4 = typename _vec<ulong, 4>;
		using Ulong4c = typename _vec_constexpr<ulong, 4>;
		using Long4 = typename _vec<slong, 4>;
		using Long4c = typename _vec_constexpr<slong, 4>;
		using Float4 = typename _vec<float, 4>;
		using Float4c = typename _vec_constexpr<float, 4>;
		using Double4 = typename _vec<double, 4>;
		using Double4c = typename _vec_constexpr<double, 4>;
		using Bool4 = typename _vec<bool, 4>;
		using Bool4c = typename _vec_constexpr<bool, 4>;

		template<IsBaseNumber T1, IsBaseNumber T2>
		//返回类型为自动
		DME_INLINE constexpr auto Min(const _vec<T1, 2>& a, const _vec<T2, 2>& b) noexcept {
			return _vec<arithmetic_return_type<T1, T2>, 2>(Min(a.x, b.x), Min(a.y, b.y));
		}
		template<IsBaseNumber R, IsBaseNumber T1, IsBaseNumber T2>
		//手动指定返回类型
		DME_INLINE constexpr auto MinC(const _vec<T1, 2>& a, const _vec<T2, 2>& b) noexcept {
			return _vec<R, 2>(MinC<R>(a.x, b.x), MinC<R>(a.y, b.y));
		}
		template<IsBaseNumber T1, IsBaseNumber T2>
		//返回类型为自动
		DME_INLINE constexpr auto Max(const _vec<T1, 2>& a, const _vec<T2, 2>& b) noexcept {
			return _vec<arithmetic_return_type<T1, T2>, 2>(Max(a.x, b.x), Max(a.y, b.y));
		}
		template<IsBaseNumber R, IsBaseNumber T1, IsBaseNumber T2>
		//手动指定返回类型
		DME_INLINE constexpr auto MaxC(const _vec<T1, 2>& a, const _vec<T2, 2>& b) noexcept {
			return _vec<R, 2>(MaxC<R>(a.x, b.x), MaxC<R>(a.y, b.y));
		}
		template<IsBaseNumber T>
		DME_INLINE constexpr auto Abs(const _vec<T, 2>& a) noexcept {
			return _vec<T, 2>(Abs(a.x), Abs(a.y));
		}
		template<IsBaseNumber R, IsBaseNumber T1, IsBaseNumber T2>
		//返回类型为第一个参数的类型
		DME_INLINE constexpr auto Clamp(const _vec<R, 2>& a, const _vec<T1, 2>& low, const _vec<T2, 2>& high) noexcept {
			return _vec<R, 2>(Clamp(a.x, low.x, high.x), Clamp(a.y, low.y, high.y));
		}

		template<IsBaseNumber T1, IsBaseNumber T2>
		//返回类型为自动
		DME_INLINE constexpr auto Min(const _vec<T1, 3>& a, const _vec<T2, 3>& b) noexcept {
			return _vec<arithmetic_return_type<T1, T2>, 3>(Min(a.x, b.x), Min(a.y, b.y), Min(a.z, b.z));
		}
		template<IsBaseNumber R, IsBaseNumber T1, IsBaseNumber T2>
		//手动指定返回类型
		DME_INLINE constexpr auto MinC(const _vec<T1, 3>& a, const _vec<T2, 3>& b) noexcept {
			return _vec<R, 3>(MinC<R>(a.x, b.x), MinC<R>(a.y, b.y), MinC<R>(a.z, b.z));
		}
		template<IsBaseNumber T1, IsBaseNumber T2>
		//返回类型为自动
		DME_INLINE constexpr auto Max(const _vec<T1, 3>& a, const _vec<T2, 3>& b) noexcept {
			return _vec<arithmetic_return_type<T1, T2>, 3>(Max(a.x, b.x), Max(a.y, b.y), Max(a.z, b.z));
		}
		template<IsBaseNumber R, IsBaseNumber T1, IsBaseNumber T2>
		//手动指定返回类型
		DME_INLINE constexpr auto MaxC(const _vec<T1, 3>& a, const _vec<T2, 3>& b) noexcept {
			return _vec<R, 3>(Max<R>(a.x, b.x), Max<R>(a.y, b.y), Max<R>(a.z, b.z));
		}
		template<IsBaseNumber T>
		DME_INLINE constexpr auto Abs(const _vec<T, 3>& a) noexcept {
			return _vec<T, 3>(Abs(a.x), Abs(a.y), Abs(a.z));
		}
		template<IsBaseNumber R, IsBaseNumber T1, IsBaseNumber T2>
		//返回类型为第一个参数的类型
		DME_INLINE constexpr auto Clamp(const _vec<R, 3>& a, const _vec<T1, 3>& low, const _vec<T2, 3>& high) noexcept {
			return _vec<R, 3>(Clamp(a.x, low.x, high.x), Clamp(a.y, low.y, high.y), Clamp(a.z, low.z, high.z));
		}

		template<IsBaseNumber T1, IsBaseNumber T2>
		//返回类型为自动
		DME_INLINE constexpr auto Min(const _vec<T1, 4>& a, const _vec<T1, 4>& b) noexcept {
			return _vec<arithmetic_return_type<T1, T2>, 4>(Min(a.x, b.x), Min(a.y, b.y), Min(a.z, b.z), Min(a.w, b.w));
		}
		template<IsBaseNumber R, IsBaseNumber T1, IsBaseNumber T2>
		//手动指定返回类型
		DME_INLINE constexpr auto MinC(const _vec<T1, 4>& a, const _vec<T1, 4>& b) noexcept {
			return _vec<R, 4>(MinC<R>(a.x, b.x), MinC<R>(a.y, b.y), MinC<R>(a.z, b.z), MinC<R>(a.w, b.w));
		}
		template<IsBaseNumber T1, IsBaseNumber T2>
		//返回类型为自动
		DME_INLINE constexpr auto Max(const _vec<T1, 4>& a, const _vec<T1, 4>& b) noexcept {
			return _vec<arithmetic_return_type<T1, T2>, 4>(Max(a.x, b.x), Max(a.y, b.y), Max(a.z, b.z), Max(a.w, b.w));
		}
		template<IsBaseNumber R, IsBaseNumber T1, IsBaseNumber T2>
		//手动指定返回类型
		DME_INLINE constexpr auto MaxC(const _vec<T1, 4>& a, const _vec<T1, 4>& b) noexcept {
			return _vec<R, 4>(MaxC<R>(a.x, b.x), MaxC<R>(a.y, b.y), MaxC<R>(a.z, b.z), MaxC<R>(a.w, b.w));
		}
		template<IsBaseNumber T>
		DME_INLINE constexpr auto Abs(const _vec<T, 4>& a) noexcept {
			return _vec<T, 4>(Abs(a.x), Abs(a.y), Abs(a.z), Abs(a.w));
		}
		template<IsBaseNumber R, IsBaseNumber T1, IsBaseNumber T2>
		//返回类型为第一个参数的类型
		DME_INLINE constexpr auto Clamp(const _vec<R, 4>& a, const _vec<T1, 4>& low, const _vec<T2, 4>& high) noexcept {
			return _vec<R, 4>(Clamp(a.x, low.x, high.x), Clamp(a.y, low.y, high.y), Clamp(a.z, low.z, high.z), Clamp(a.w, low.w, high.w));
		}

		//仅用于模板实例化
		static void _vec_init() {
			Uchar2(); Uchar2c();
			Char2(); Char2c();
			Ushort2(); Ushort2c();
			Short2(); Short2c();
			Uint2(); Uint2c();
			Int2(); Int2c();
			Ulong2(); Ulong2c();
			Long2(); Long2c();
			Float2(); Float2c();
			Double2(); Double2c();
			Bool2(); Bool2c();

			Uchar3(); Uchar3c();
			Char3(); Char3c();
			Ushort3(); Ushort3c();
			Short3(); Short3c();
			Uint3(); Uint3c();
			Int3(); Int3c();
			Ulong3(); Ulong3c();
			Long3(); Long3c();
			Float3(); Float3c();
			Double3(); Double3c();
			Bool3(); Bool3c();

			Uchar4(); Uchar4c();
			Char4(); Char4c();
			Ushort4(); Ushort4c();
			Short4(); Short4c();
			Uint4(); Uint4c();
			Int4(); Int4c();
			Ulong4(); Ulong4c();
			Long4(); Long4c();
			Float4(); Float4c();
			Double4(); Double4c();
			Bool4(); Bool4c();
		}
	}

	using math::Uchar2; using math::Uchar3; using math::Uchar4;
	using math::Uchar2c; using math::Uchar3c; using math::Uchar4c;
	using math::Char2; using math::Char3; using math::Char4;
	using math::Char2c; using math::Char3c; using math::Char4c;

	using math::Ushort2; using math::Ushort3; using math::Ushort4;
	using math::Ushort2c; using math::Ushort3c; using math::Ushort4c;
	using math::Short2; using math::Short3; using math::Short4;
	using math::Short2c; using math::Short3c; using math::Short4c;

	using math::Uint2; using math::Uint3; using math::Uint4;
	using math::Uint2c; using math::Uint3c; using math::Uint4c;
	using math::Int2; using math::Int3; using math::Int4;
	using math::Int2c; using math::Int3c; using math::Int4c;

	using math::Long2; using math::Long3; using math::Long4;
	using math::Long2c; using math::Long3c; using math::Long4c;
	using math::Ulong2; using math::Ulong3; using math::Ulong4;
	using math::Ulong2c; using math::Ulong3c; using math::Ulong4c;

	using math::Float2; using math::Float3; using math::Float4;
	using math::Float2c; using math::Float3c; using math::Float4c;
	using math::Double2; using math::Double3; using math::Double4;
	using math::Double2c; using math::Double3c; using math::Double4c;

	using math::Bool2; using math::Bool3; using math::Bool4;
	using math::Bool2c; using math::Bool3c; using math::Bool4c;
}

namespace std {
	template<typename T, dme::math::axis_t A>
	struct hash<dme::math::_vec<T, A>> {
		size_t operator ()(const dme::math::_vec<T, A>& a) const noexcept {
			return Hasher(a);
		}
	};
}

