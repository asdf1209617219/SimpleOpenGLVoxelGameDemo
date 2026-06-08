#pragma once
#include <dme/math/vector.h>
namespace dme {
	namespace math {
		//2d包围盒的边。x+为右，y+为前
		struct Side2 {
			uchar v;

			constexpr Side2() noexcept : v(0) {};
			constexpr Side2(uchar v) noexcept : v(v) {}

			static constexpr uchar Right = 0; //0b000
			static constexpr uchar Left = 1; //0b001
			static constexpr uchar Top = 2; //0b010
			static constexpr uchar Bottom = 3; //0b011

			static constexpr uchar Min = Right;
			static constexpr uchar Max = Bottom;
			static constexpr uchar Len = Max + 1;
			static constexpr uint bit_width = BitWidth(Max); //最多可能占用的bit数

			inline constexpr operator uchar() const noexcept {
				return v;
			}
			friend std::ostream& operator<<(std::ostream& os, const Side2& a) {
				switch (a.v) {
				case Right:
					return os << "Right";
				case Left:
					return os << "Left";
				case Top:
					return os << "Top";
				case Bottom:
					return os << "Bottom";
				default:
					return os << "Unknown";
				}
			}
			friend constexpr Hash Hasher(const Side2& a) noexcept {
				return Hasher(a.v);
			}
		};
		struct Side2c {
			static constexpr Side2 Right = Side2::Right; //0b000
			static constexpr Side2 Left = Side2::Left; //0b001
			static constexpr Side2 Top = Side2::Top; //0b010
			static constexpr Side2 Bottom = Side2::Bottom; //0b011

			static constexpr Side2 Min = Side2::Min;
			static constexpr Side2 Max = Side2::Max;
			static constexpr Side2 Len = Side2::Len;
		};

		//3d包围盒的表面。x+为右，y+为前
		struct Face3 {
			uchar v;

			constexpr Face3() noexcept : v(0) {};
			constexpr Face3(uchar v) noexcept : v(v) {}

			static constexpr uchar Right = 0; //0b000
			static constexpr uchar Left = 1; //0b001
			static constexpr uchar Front = 2; //0b010
			static constexpr uchar Back = 3; //0b011
			static constexpr uchar Top = 4; //0b100
			static constexpr uchar Bottom = 5; //0b101

			static constexpr uchar Min = Right;
			static constexpr uchar Max = Bottom;
			static constexpr uchar Len = Max + 1;
			static constexpr uint bit_width = BitWidth(Max); //最多可能占用的bit数

			//是否在正轴上
			inline constexpr bool isPositive() const noexcept {
				return !(v & 1);
			}
			//是否在负轴上
			inline constexpr bool isNegative() const noexcept {
				return v & 1;
			}
			//获取面所在轴
			inline constexpr Axis3 axis() const noexcept {
				return Axis3(v >> 1);
			}
			//获取面的反面
			inline constexpr Face3 getInvert() const noexcept {
				return Face3(v ^ 1);
			}
			//变为反面
			inline Face3 setInvert() noexcept {
				v = v ^ 1;
			}
			//获取某个位置向当前面朝向移动的下一个位置
			inline constexpr Int3 nearPos(const Int3& pos) const noexcept {
				Int3 r = Int3(pos);
				r.v[axis()] += negativeOne(isNegative());
				return r;
			}

			//遍历每个面并执行函数(只能在函数体内使用return来达到continue的效果，无法使用break)
			static inline void each(const std::function<void(Face3 face)>& func);

			inline constexpr operator uchar() const noexcept {
				return v;
			}
			friend std::ostream& operator<<(std::ostream& os, const Face3& a) {
				switch (a.v) {
				case Right:
					return os << "Right";
				case Left:
					return os << "Left";
				case Front:
					return os << "Front";
				case Back:
					return os << "Back";
				case Top:
					return os << "Top";
				case Bottom:
					return os << "Bottom";
				default:
					return os << "Unknown";
				}
			}
			friend constexpr Hash Hasher(const Face3& a) noexcept {
				return Hasher(a.v);
			}
		};
		struct Face3c {
			static constexpr Face3 Right = Face3::Right; //0b000
			static constexpr Face3 Left = Face3::Left; //0b001
			static constexpr Face3 Front = Face3::Front; //0b010
			static constexpr Face3 Back = Face3::Back; //0b011
			static constexpr Face3 Top = Face3::Top; //0b100
			static constexpr Face3 Bottom = Face3::Bottom; //0b101

			static constexpr Face3 Min = Face3::Min;
			static constexpr Face3 Max = Face3::Max;
			static constexpr Face3 Len = Face3::Len;
		};

		inline void Face3::each(const std::function<void(Face3 face)>& func) {
			func(Face3c::Right);
			func(Face3c::Left);
			func(Face3c::Front);
			func(Face3c::Back);
			func(Face3c::Top);
			func(Face3c::Bottom);
		}

		//表面的旋转
		struct FRotate {
			uchar v;

			constexpr FRotate() noexcept : v(0) {};
			constexpr FRotate(uchar v) noexcept : v(v) {}

			static constexpr uchar N = 0; //不旋转(none)
			static constexpr uchar CW = 1; //顺时针(clockwise)
			static constexpr uchar CS = 2; //中心对称(central symmetry)
			static constexpr uchar CCW = 3; //逆时针(counterclockwise)

			static constexpr uchar Min = N;
			static constexpr uchar Max = CCW;
			static constexpr uchar Len = Max + 1;
			static constexpr uint bit_width = BitWidth(Max); //最多可能占用的bit数

			//获取下一个旋转（顺时针90度）
			constexpr FRotate next() const noexcept {
				return FRotate((v + 1) & 0b11);
			}
			//获取与之对称的旋转
			constexpr FRotate symmetry() const noexcept {
				return FRotate(v ^ 0b10);
			}
			//获取前一个旋转（逆时针90度）
			constexpr FRotate previous() const noexcept {
				return FRotate((v + 3) & 0b11);
			}
			//获取该旋转按某个方向旋转多少次后的旋转
			constexpr FRotate directionCount(FRotate direction, uchar count) const noexcept {
				return FRotate((v + count * direction) & 0b11);
			}
			//获取旋转后的纹理坐标点数组的下标(一般有4个，从左下角开始顺时针依次指向正方形的4个角)
			constexpr uint texIndex(uchar originTexIndex) const noexcept {
				return (originTexIndex + v * 3) & 0b11; //其实就是0,3,2,1这个顺序，v对于纹理坐标点数组下标相当于倒着数的次数
			}

			inline constexpr operator uchar() const noexcept {
				return v;
			}
			friend std::ostream& operator<<(std::ostream& os, const FRotate& a) {
				return os << +a.v;
			}
			friend constexpr Hash Hasher(const FRotate& a) noexcept {
				return Hasher(a.v);
			}
		};

		struct FRotatec {
			static constexpr FRotate N = FRotate::N; //不旋转(none)
			static constexpr FRotate CW = FRotate::CW; //顺时针(clockwise)
			static constexpr FRotate CS = FRotate::CS; //中心对称(central symmetry)
			static constexpr FRotate CCW = FRotate::CCW; //逆时针(counterclockwise)

			static constexpr FRotate Min = FRotate::Min;
			static constexpr FRotate Max = FRotate::Max;
			static constexpr FRotate Len = FRotate::Len;
		};
	}
	using math::Side2;
	using math::Side2c;
	using math::Face3;
	using math::Face3c;
	using math::FRotate;
	using math::FRotatec;
}

namespace std {
	template<>
	struct hash<dme::math::Side2> {
		inline size_t operator ()(const dme::math::Side2& a) const noexcept {
			return Hasher(a);
		}
	};
	template<>
	struct hash<dme::math::Face3> {
		inline size_t operator ()(const dme::math::Face3& a) const noexcept {
			return Hasher(a);
		}
	};
	template<>
	struct hash<dme::math::FRotate> {
		inline size_t operator ()(const dme::math::FRotate& a) const noexcept {
			return Hasher(a);
		}
	};
}