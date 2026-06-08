#pragma once
#include <dme/block/bpos.h>
namespace dme::block {
	//表示对于长方体的相对方位标识
	enum class CubeDirectionFlag : uint {
		in = 0b1 << 0,
		xp = 0b1 << 1,
		xn = 0b1 << 2,
		yp = 0b1 << 3,
		yn = 0b1 << 4,
		zp = 0b1 << 5,
		zn = 0b1 << 6,
		xpyp = 0b1 << 7,
		xnyp = 0b1 << 8,
		xpyn = 0b1 << 9,
		xnyn = 0b1 << 10,
		xpzp = 0b1 << 11,
		xnzp = 0b1 << 12,
		xpzn = 0b1 << 13,
		xnzn = 0b1 << 14,
		ypzp = 0b1 << 15,
		ynzp = 0b1 << 16,
		ypzn = 0b1 << 17,
		ynzn = 0b1 << 18,
		xpypzp = 0b1 << 19,
		xnypzp = 0b1 << 20,
		xpynzp = 0b1 << 21,
		xnynzp = 0b1 << 22,
		xpypzn = 0b1 << 23,
		xnypzn = 0b1 << 24,
		xpynzn = 0b1 << 25,
		xnynzn = 0b1 << 26,
	};

	enum class CompareFlag : uchar {
		equal = 0,
		less = 1,
		greater = 2,
	};

	//分别比较3个轴大小获得的结果
	struct BlockPosCompare {
		uchar value;

		constexpr BlockPosCompare() noexcept : value(0) {}
		constexpr BlockPosCompare(uchar value) noexcept : value(value) {}
		constexpr BlockPosCompare(const Int3& a, const Int3& b) noexcept
			: value(0) {
			uchar x_ = a.x > b.x ? 0b10 : (a.x < b.x ? 0b01 : 0);
			uchar y_ = a.y > b.y ? 0b1000 : (a.y < b.y ? 0b0100 : 0);
			uchar z_ = a.z > b.z ? 0b100000 : (a.z < b.z ? 0b010000 : 0);
			uchar c_ = ((x_ ? 1 : 0) + (y_ ? 1 : 0) + (z_ ? 1 : 0)) << 6;
			value = x_ | y_ | z_ | c_;
		}

		constexpr CompareFlag x() const noexcept;
		constexpr CompareFlag y() const noexcept;
		constexpr CompareFlag z() const noexcept;
		constexpr uchar notEqualCount() const noexcept;
		constexpr bool in() const noexcept;
		constexpr bool xEqual() const noexcept;
		constexpr bool yEqual() const noexcept;
		constexpr bool zEqual() const noexcept;
		constexpr bool xLess() const noexcept;
		constexpr bool yLess() const noexcept;
		constexpr bool zLess() const noexcept;
		constexpr bool xGreater() const noexcept;
		constexpr bool yGreater() const noexcept;
		constexpr bool zGreater() const noexcept;
	};

	//BlockCuboid的位置，分别比较3个轴大小获得的结果
	struct BlockCuboidCompare {
		//ffffffcczzyyxx。从左至右，6bit用于按大小保存3个轴的顺序最小的在左边，2bit为在长方体内部的轴的数量，6bit为每个轴的比较结果00为大于01为小于10为处于该轴对应的范围内部
		uint value;

		constexpr BlockCuboidCompare() noexcept : value(0) {}
		constexpr BlockCuboidCompare(uint value) noexcept : value(value) {}

		//返回x的比较结果，-1为小于，0为等于，1为大于
		constexpr sint x() const noexcept;
		//返回y的比较结果，-1为小于，0为等于，1为大于
		constexpr sint y() const noexcept;
		//返回z的比较结果，-1为小于，0为等于，1为大于
		constexpr sint z() const noexcept;
		//返回某个轴的比较结果，-1为小于，0为等于，1为大于
		constexpr sint i(uint index) const noexcept;

		//返回x的比较结果的实际编码，0为小于，2为等于，1为大于
		constexpr uint xv() const noexcept;
		//返回y的比较结果的实际编码，0为小于，2为等于，1为大于
		constexpr uint yv() const noexcept;
		//返回z的比较结果的实际编码，0为小于，2为等于，1为大于
		constexpr uint zv() const noexcept;
		//返回某个轴的比较结果的实际编码，0为小于，2为等于，1为大于
		constexpr uint iv(uint index) const noexcept;
		//返回不等于的数量
		constexpr uint notEqualCount() const noexcept;

		constexpr bool xGreaterEqual() const noexcept;
		constexpr bool yGreaterEqual() const noexcept;
		constexpr bool zGreaterEqual() const noexcept;
		constexpr bool iGreaterEqual(uint index) const noexcept;

		//返回面的顺序，比如BlockPos位于6个面时返回对应的面，位于边上时返回两个面，角上则返回三个（固定为xyz）
		constexpr Uint3 faces() const noexcept;
		//BlockPos是否在BlockCuboid内部
		constexpr bool in() const noexcept;

		//打印比较的方块在长方体的相对位置
		friend std::ostream& operator<<(std::ostream& os, const BlockCuboidCompare& a);
	};

	struct FixCuboidCompare {
		BlockCuboidCompare result;
		Fix3 pos;

		constexpr FixCuboidCompare() noexcept : result(), pos() {}
		constexpr FixCuboidCompare(BlockCuboidCompare result, Fix3 pos) noexcept : result(result), pos(pos) {}
	};

	//两个长方体向量的差（向量方向必须相同）
	struct BlockCuboidVectorDiff {
		//两个长方体重合部分的起始点
		Int3 start;
		//重合部分长方体的长度，具有方向性，可为负数或零
		Int3 length;
		//三个方向延伸的长度，具有方向性，可为负数或零
		Int3 extend;

		constexpr BlockCuboidVectorDiff() noexcept : start(), length(), extend() {}
		constexpr BlockCuboidVectorDiff(const Int3& start, const Int3& length, const Int3& extend) noexcept : start(start), length(length), extend(extend) {}
	};

	//长方体
	struct FixCuboid {
		Fix3 min;
		Fix3 max;

		constexpr FixCuboid() noexcept : min(), max() {}
		constexpr FixCuboid(const Fix3& min, const Fix3& max) noexcept : min(min), max(max) {}

		constexpr FixCuboid(const Fix3& center, const Float3& half) noexcept : min(center - half), max(center + half) {}

		constexpr FixCuboidCompare compare(const Int3& blockPos) const noexcept;
		//根据比较结果获取对应的面或边或角的位置
		constexpr Fix3 getResultPos(const BlockCuboidCompare& result) const noexcept;

		constexpr bool operator ==(const FixCuboid& a) const noexcept;
		constexpr bool operator !=(const FixCuboid& a) const noexcept;
		friend std::ostream& operator<<(std::ostream& os, const FixCuboid& a);

		friend constexpr Hash Hasher(const FixCuboid& a) noexcept {
			return Hash::Combine(a.min, a.max);
		}
	};

	//多个方块组成的长方体
	struct BlockCuboid {
		Int3 min;
		Int3 max;

		constexpr BlockCuboid() noexcept : min(), max() {}
		constexpr BlockCuboid(Int3 min, Int3 max) noexcept : min(min), max(max) {}
		constexpr BlockCuboid(const Fix3& start, const Fix3& end, bool hasBottom = false) noexcept : min(), max() {
			if (start.x > end.x) {
				min.x = end.x;
				max.x = start.x;
				if (min.x != max.x && start.xf == 0) {
					max.x--;
				}
			}
			else {
				min.x = start.x;
				max.x = end.x;
				if (min.x != max.x && end.xf == 0) {
					max.x--;
				}
			}
			if (start.y > end.y) {
				min.y = end.y;
				max.y = start.y;
				if (min.y != max.y && start.yf == 0) {
					max.y--;
				}
			}
			else {
				min.y = start.y;
				max.y = end.y;
				if (min.y != max.y && end.yf == 0) {
					max.y--;
				}
			}
			if (start.z > end.z) {
				min.z = end.z;
				max.z = start.z;
				if (min.z != max.z) {
					if (start.zf == 0) {
						max.z--;
					}
					if (hasBottom && end.zf == 0) { //最下方贴在地面时，下面一层也加入进来
						min.z--;
					}
				}
			}
			else {
				min.z = start.z;
				max.z = end.z;
				if (min.z != max.z) {
					if (end.zf == 0) {
						max.z--;
					}
					if (hasBottom && start.zf == 0) { //最下方贴在地面时，下面一层也加入进来
						min.z--;
					}
				}
			}
		}
		constexpr BlockCuboid(const block::FixCuboid& fixCuboid, bool hasBottom = false) noexcept : min(), max() {
			min.x = fixCuboid.min.x;
			min.y = fixCuboid.min.y;
			min.z = fixCuboid.min.z;
			max.x = fixCuboid.max.x;
			max.y = fixCuboid.max.y;
			max.z = fixCuboid.max.z;
			if (min.x != max.x && fixCuboid.max.xf == 0) {
				max.x--;
			}
			if (min.y != max.y && fixCuboid.max.yf == 0) {
				max.y--;
			}
			if (min.z != max.z && fixCuboid.max.zf == 0) {
				max.z--;
			}
			if (hasBottom && fixCuboid.min.zf == 0) { //最下方贴在地面时，下面一层也加入进来
				min.z--;
			}
		}

		constexpr BlockCuboid(const Fix3& center, const Float3& half, bool hasBottom = false) noexcept : min(), max() {
			Fix3 min_ = center - half;
			Fix3 max_ = center + half;
			if (min_.x != max_.x && max_.xf == 0) {
				max_.x--;
			}
			if (min_.y != max_.y && max_.yf == 0) {
				max_.y--;
			}
			if (min_.z != max_.z && max_.zf == 0) {
				max_.z--;
			}
			if (hasBottom && min_.zf == 0) { //最下方贴在地面时，下面一层也加入进来
				min_.z--;
			}
			min.x = min_.x;
			min.y = min_.y;
			min.z = min_.z;
			max.x = max_.x;
			max.y = max_.y;
			max.z = max_.z;
		}

		constexpr BlockCuboid(const BlockCuboid& a) noexcept : min(a.min), max(a.max) {}
		constexpr BlockCuboid(BlockCuboid&& a) noexcept : min(a.min), max(a.max) {}

		constexpr BlockCuboidCompare compare(const Int3& blockPos) const noexcept;
		//根据比较结果获取对应的面或边或角的位置
		constexpr Int3 getRealPos(const BlockCuboidCompare& result) const noexcept;

		constexpr bool operator ==(const BlockCuboid& a) const noexcept;
		constexpr bool operator !=(const BlockCuboid& a) const noexcept;
		friend std::ostream& operator<<(std::ostream& os, const BlockCuboid& a);
		friend constexpr Hash Hasher(const BlockCuboid& a) noexcept {
			return Hash::Combine(a.min, a.max);
		}
	};


	//具有方向性的多个方块组成的长方体
	struct BlockCuboidVector {
		Int3 start;
		Int3 end;

		constexpr BlockCuboidVector() noexcept : start(), end() {}
		constexpr BlockCuboidVector(Int3 start, Int3 end) noexcept : start(start), end(end) {}

		//注意传入的参数的朝向必须与当前对象的相同
		constexpr BlockCuboidVectorDiff diff(const BlockCuboidVector& a) const noexcept;
	};

	//用于描述AABB的移动以及需要检测的方块范围
	struct AABBMove {
	private:
		constexpr AABBMove(Fix3& center, const Float3& half, const Float3& delta) noexcept {
			unit = delta.toSign(); //与delta每个轴的朝向相同的单位向量
			absDelta = math::Abs(delta);
			Float3 signHalf = half * unit;

			oldPosStart = center - signHalf;
			oldPosEnd = center + signHalf;
			center += delta;
			newPosStart = center - signHalf;
			newPosEnd = center + signHalf;

			inner = block::CuboidCornerToBlockPos(oldPosEnd, unit) + unit;
			blockStart = block::CuboidCornerToBlockPos(newPosStart, -unit);
			blockEnd = block::CuboidCornerToBlockPos(newPosEnd, unit) + unit;
		}
	public:
		//移动前长方体的起始点
		Fix3 oldPosStart;
		//移动前长方体的结束点
		Fix3 oldPosEnd;
		//移动后长方体的起始点
		Fix3 newPosStart;
		//移动后长方体的结束点
		Fix3 newPosEnd;

		//移动距离的绝对值
		Float3 absDelta;
		//遍历时位置移动的单位（与delta的正负相同，绝对值为1）
		Int3 unit;

		//旧方块结束点到新方块结束点组成的长方体的起点
		Int3 inner;
		//新方块起始点
		Int3 blockStart;
		//新方块结束点+unit
		Int3 blockEnd;

		//移动AABB，会修改center的值
		static constexpr AABBMove Move(Fix3& center, const Float3& half, const Float3& delta) noexcept {
			return AABBMove(center, half, delta);
		}
	};

}

namespace std {
	template<>
	struct hash<dme::block::FixCuboid> {
		size_t operator ()(const dme::block::FixCuboid& a) const noexcept {
			return Hasher(a);
		}
	};
	template<>
	struct hash<dme::block::BlockCuboid> {
		size_t operator ()(const dme::block::BlockCuboid& a) const noexcept {
			return Hasher(a);
		}
	};
}