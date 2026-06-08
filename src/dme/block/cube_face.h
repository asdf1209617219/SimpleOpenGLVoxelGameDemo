#pragma once
#include <dme/core.h>
namespace dme::block {
	class BlockBase;

	struct Face3Rotate {
		Face3 face;
		FRotate rotate;

		constexpr Face3Rotate() noexcept : face(), rotate() {};
		constexpr Face3Rotate(Face3 face, FRotate rotate) noexcept : face(face), rotate(rotate) {};
	};

	//表示每个面的隐藏情况
	struct CubeFaceDisplay {
		uchar v;

		constexpr CubeFaceDisplay() noexcept : v(0b111111) {};
		explicit constexpr CubeFaceDisplay(uchar displays) noexcept : v(displays) {};
		constexpr CubeFaceDisplay(bool right, bool left, bool front, bool back, bool top, bool bottom) noexcept : v(
			(static_cast<uchar>(right))
			| (static_cast<uchar>(left) << 1)
			| (static_cast<uchar>(front) << 2)
			| (static_cast<uchar>(back) << 3)
			| (static_cast<uchar>(top) << 4)
			| (static_cast<uchar>(bottom) << 5)
		) {
		};

		explicit constexpr operator uchar() const noexcept {
			return v;
		}

		//默认全都显示
		constexpr uchar defaultDisplay() const noexcept {
			return 0b111111;
		}
		constexpr bool rightIsDisplay() const noexcept {
			return v & 0b100000;
		}
		constexpr bool leftIsDisplay() const noexcept {
			return v & 0b10000;
		}
		constexpr bool frontIsDisplay() const noexcept {
			return v & 0b1000;
		}
		constexpr bool backIsDisplay() const noexcept {
			return v & 0b100;
		}
		constexpr bool topIsDisplay() const noexcept {
			return v & 0b10;
		}
		constexpr bool bottomIsDisplay() const noexcept {
			return v & 0b1;
		}
		constexpr bool isDisplay(uchar face) const noexcept {
			return v & (0b100000 >> face);
		}
		constexpr bool isDisplay(Face3 face) const noexcept {
			return v & (0b100000 >> face);
		}
		constexpr bool isHide(uchar face) const noexcept {
			return !(v & (0b100000 >> face));
		}
		constexpr bool isHide(Face3 face) const noexcept {
			return !(v & (0b100000 >> face));
		}
		constexpr void display(uchar face) noexcept {
			v |= 0b100000 >> face;
		}
		constexpr void display(Face3 face) noexcept {
			v |= 0b100000 >> face;
		}
		constexpr void hide(uchar face) noexcept {
			v &= 0b111111 ^ (0b100000 >> face);
		}
		constexpr void hide(Face3 face) noexcept {
			v &= 0b111111 ^ (0b100000 >> face);
		}
		constexpr uchar displayCount() const noexcept {
			return (v & 0b1)
				+ ((v >> 1) & 0b1)
				+ ((v >> 2) & 0b1)
				+ ((v >> 3) & 0b1)
				+ ((v >> 4) & 0b1)
				+ ((v >> 5) & 0b1);
		}
	};

	//表示每个面的自旋情况
	struct CubeFaceSpin {
		ushort v;

		constexpr CubeFaceSpin() noexcept : v(0) {}
		constexpr CubeFaceSpin(ushort rotations) noexcept : v(rotations) {}
		constexpr CubeFaceSpin(FRotate right, FRotate left, FRotate front, FRotate back, FRotate top, FRotate bottom) noexcept : v(
			(right << 10)
			| (left << 8)
			| (front << 6)
			| (back << 4)
			| (top << 2)
			| (bottom)
		) {
		}

		//设置为默认，默认全都不旋转
		ushort setDefault() noexcept {
			v = 0;
		}
		constexpr FRotate right() const noexcept {
			return (v >> 10) & 0b11;
		}
		constexpr FRotate left() const noexcept {
			return (v >> 8) & 0b11;
		}
		constexpr FRotate front() const noexcept {
			return (v >> 6) & 0b11;
		}
		constexpr FRotate back() const noexcept {
			return (v >> 4) & 0b11;
		}
		constexpr FRotate top() const noexcept {
			return (v >> 2) & 0b11;
		}
		constexpr FRotate bottom() const noexcept {
			return v & 0b11;
		}
		constexpr FRotate get(Face3 face) const noexcept {
			return (v >> (10 - 2 * face)) & 0b11;
		}
		constexpr void set(Face3 face, FRotate rotate) noexcept {
			uchar offset = 10 - 2 * face;
			v = (v & ~(0b11 << offset)) | (rotate << offset);
		}
		//将该面顺时针转
		constexpr void cwSpin(Face3 face) noexcept {
			uchar offset = 10 - 2 * face;
			uchar rotate = ((v >> offset) + 1) & 0b11;
			v = (v & ~(0b11 << offset)) | (rotate << offset);
		}
		//获取将该面顺时针转后的数据
		constexpr FRotate getCwSpin(Face3 face) const noexcept {
			uchar offset = 10 - 2 * face;
			uchar rotate = ((v >> offset) + 1) & 0b11;
			return (v & ~(0b11 << offset)) | (rotate << offset);
		}
		//将该面逆时针转
		constexpr void ccwSpin(Face3 face) noexcept {
			uchar offset = 10 - 2 * face;
			uchar rotate = ((v >> offset) + 3) & 0b11;
			v = (v & ~(0b11 << offset)) | (rotate << offset);
		}
		//获取将该面逆时针转后的数据
		constexpr FRotate getCcwSpin(Face3 face) const noexcept {
			uchar offset = 10 - 2 * face;
			uchar rotate = ((v >> offset) + 3) & 0b11;
			return (v & ~(0b11 << offset)) | (rotate << offset);
		}
		//将该面中心对称
		constexpr void csSpin(Face3 face) noexcept {
			v ^= 0b10 << (10 - 2 * face);
		}
		//获取将该面中心对称后的数据
		constexpr FRotate getCsSpin(Face3 face) const noexcept {
			return v ^ (0b10 << (10 - 2 * face));
		}

	};

	//表示方块整体的旋转方向，向右为x+向前为y+向上为z+是默认方向
	struct CubeFaceToward {
		uchar v;

		static constexpr uchar mask = Face3::bit_width + FRotate::bit_width;
		static constexpr uchar frotate_mask = BitFull(FRotate::bit_width);
		static constexpr uchar face3_mask = BitFull(Face3::bit_width) << FRotate::bit_width;

		constexpr CubeFaceToward() noexcept : v(0) {};
		explicit constexpr CubeFaceToward(uchar toward) noexcept : v(toward) {}
		constexpr CubeFaceToward(Face3 rightToward, FRotate rotate) noexcept : v((rightToward << FRotate::bit_width) | rotate) {}

		inline explicit constexpr operator uchar() const noexcept {
			return v;
		}
		inline explicit constexpr operator uint() const noexcept {
			return v;
		}

		//默认右面为x+，上面为z+
		inline constexpr void setDefault() noexcept {
			v = 0;
		}
		//旋转方式：先根据z轴进行旋转，将x+面旋转到FaceIndex对应面；如果要将x+面转到上或下面，则根据y轴旋转将x+面转到对应位置，并且再根据x轴旋转，将x+面的纹理坐标旋转为默认原本该面的纹理坐标
		//然后以旋转后的右面向外为旋转轴正方向顺时针旋转FaceRotate次，所有情况一共24种可能
		inline constexpr Face3 getRightToward() const noexcept {
			return Face3(v >> FRotate::bit_width);
		}
		inline constexpr FRotate getFRotate() const noexcept {
			return FRotate(v & frotate_mask);
		}
		inline constexpr void setRightToward(Face3 rightToward) noexcept {
			v = (v & frotate_mask) | (rightToward << FRotate::bit_width);
		}
		inline constexpr void setFRotate(FRotate rotate) noexcept {
			v = (v & face3_mask) | rotate;
		}
		//将当前面顺时针转
		inline constexpr void cwRotate() noexcept {
			v = (v & face3_mask) | ((v + 1) & frotate_mask);
		}
		//将当前面逆时针转
		inline constexpr void ccwRotate() noexcept {
			v = (v & face3_mask) | ((v + 3) & frotate_mask);
		}
		//将当前面中心对称
		inline constexpr void csRotate() noexcept {
			v ^= 0b10;
		}
		//获取该正方体对应面的被旋转次数以及该面属于旋转前的哪个面，方便获取纹理坐标以及纹理ID
		Face3Rotate face3SpinToStartFace3Rotate(Face3 face, CubeFaceSpin spin) const noexcept;
		//获取该正方体对应面的被旋转次数以及该面属于旋转前的哪个面，方便获取纹理坐标以及纹理ID
		Face3Rotate face3ToStartFace3Rotate(Face3 face) const noexcept;
	};

}