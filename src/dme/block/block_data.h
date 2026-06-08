#pragma once
#include <dme/block/cube_face.h>
#include <dme/block/block_shape.h>
#include <dme/block/block_data_type.h>
namespace dme::block {

	//基础方块数据（最高位永远为0，如果不是则block判断数据类型会出现问题）
	class BlockBase {
	private:
		static constexpr uint _shape_shift_ = 3 * char_bit;
		static constexpr uint _shape_mask_ = static_cast<uint>(uchar_max) << _shape_shift_;
		static constexpr uint _base_mask_ = ~_shape_mask_;

		union {
			uint v;
			struct {
				ushort s0;
				ushort s1;
			};
			struct {
				uchar c0;
				uchar c1;
				uchar c2;
				uchar c3;
			};
		};
	public:

		constexpr BlockBase() noexcept : v(0) {};
		//explicit constexpr BlockBase(uint data) noexcept : v(data) {};

		//explicit constexpr BlockBase(BlockShape shape) noexcept : v(static_cast<uint>(shape) << _shape_shift_) {};
		explicit constexpr BlockBase(BlockShape shape) noexcept : c0(static_cast<uchar>(shape)), c1(0), c2(0), c3(0) {};

		//constexpr BlockBase(BlockShape shape, CubeFaceToward cubeFaceToward) noexcept : v((static_cast<uint>(shape) << _shape_shift_) | static_cast<uint>(cubeFaceToward)) {};
		constexpr BlockBase(BlockShape shape, CubeFaceToward cubeFaceToward) noexcept : c0(static_cast<uchar>(shape)), c1(0), c2(0), c3(static_cast<uchar>(cubeFaceToward)) {};


		//转为CubeFaceToward
		inline constexpr CubeFaceToward cubeFaceToword() const noexcept {
			//return static_cast<CubeFaceToward>(v & CubeFaceToward::mask);
			return static_cast<CubeFaceToward>(c3 & CubeFaceToward::mask);
		}
		//获取x+面所处面
		inline constexpr Face3 rightToward() const noexcept {
			return cubeFaceToword().getRightToward();
		}
		//获取x+面旋转次数
		inline constexpr FRotate faceRotate() const noexcept {
			return cubeFaceToword().getFRotate();
		}

		//获取形状
		inline constexpr BlockShape getShape() const noexcept {
			//return static_cast<BlockShape>((v & _shape_mask_) >> _shape_shift_);
			return static_cast<BlockShape>(c0);
		}
		//设置形状
		inline constexpr void setShape(BlockShape shape) noexcept {
			//v = (v & _base_mask_) | (static_cast<sint>(shape) << _shape_shift_);
			c0 = static_cast<uchar>(shape);
		}

		constexpr bool operator ==(const BlockBase& a) const noexcept {
			return v == a.v;
		}
		constexpr bool operator !=(const BlockBase& a) const noexcept {
			return v != a.v;
		}
		inline explicit constexpr operator uint() const noexcept {
			return v;
		}
		inline size_t operator ()(const BlockBase& a) const noexcept {
			std::hash<uint> hasher;
			return hasher(a.v);
		}
	};
}

namespace std {
	template<>
	struct hash<dme::block::BlockBase> {
		inline size_t operator ()(const dme::block::BlockBase& a) const noexcept {
			return a(a);
		}
	};
}