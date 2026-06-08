#pragma once
#include <dme/block/cube_face.h>
namespace dme::block {
	class Chunk;
	using ChunkMap = FlatMap<Int3, Chunk*>;

	//区块的边长对应的bit长度
	constexpr uchar chunk_length_bit_length = BitWidth(Config::chunk_length) - 1;
	//x对应bit在第几位
	constexpr uchar cbpos_x_bit_offset = 0;
	//y对应bit在第几位
	constexpr uchar cbpos_y_bit_offset = chunk_length_bit_length;
	//z对应bit在第几位
	constexpr uchar cbpos_z_bit_offset = chunk_length_bit_length << 1;


	//区块的边长
	constexpr uchar chunk_length = 1 << chunk_length_bit_length;
	//区块的面积
	constexpr ushort chunk_area = chunk_length * chunk_length;
	//区块的体积
	constexpr ushort chunk_volume = chunk_area * chunk_length;
	//区块的体积的BitWidth
	constexpr ushort chunk_volume_bit_width = BitWidth(chunk_volume);

	//区块每个坐标的最大值
	constexpr ushort chunk_side_max = chunk_length - 1;

	//取x坐标的掩码
	constexpr ushort cbpos_x_mask = chunk_side_max << cbpos_x_bit_offset;
	//取y坐标的掩码
	constexpr ushort cbpos_y_mask = chunk_side_max << cbpos_y_bit_offset;
	//取z坐标的掩码
	constexpr ushort cbpos_z_mask = chunk_side_max << cbpos_z_bit_offset;
	//去除x坐标的掩码
	constexpr ushort cbpos_remove_x_mask = cbpos_y_mask | cbpos_z_mask;
	//去除y坐标的掩码
	constexpr ushort cbpos_remove_y_mask = cbpos_x_mask | cbpos_z_mask;
	//去除z坐标的掩码
	constexpr ushort cbpos_remove_z_mask = cbpos_x_mask | cbpos_y_mask;

	//直接对x坐标操作的最小单位
	constexpr ushort cbpos_x_unit = 1 << cbpos_x_bit_offset;
	//直接对y坐标操作的最小单位
	constexpr ushort cbpos_y_unit = 1 << cbpos_y_bit_offset;
	//直接对z坐标操作的最小单位
	constexpr ushort cbpos_z_unit = 1 << cbpos_z_bit_offset;

	//区块内方块坐标bit压缩后的最大值
	constexpr ushort cbpos_max = cbpos_x_mask + cbpos_y_mask + cbpos_z_mask;

	//获取是否为null的掩码
	//constexpr uint cbpos_null_mask = 1 << 31;
	//移除null状态的掩码
	//constexpr uint cbpos_remove_null_mask = ~cbpos_null_mask;


	//Chunk中Block的位置，一区块内为chunk_length^3方块，某一方块的位置信息按zyx的顺序进行bit压缩
	struct CBPos {
		ushort v;

		constexpr CBPos() noexcept : v(0) {};
		explicit constexpr CBPos(ushort zyx) noexcept : v(zyx & cbpos_max) {}
		constexpr CBPos(ushort x, ushort y, ushort z) noexcept : v(
			((x & chunk_side_max) << cbpos_x_bit_offset)
			| ((y & chunk_side_max) << cbpos_y_bit_offset)
			| ((z & chunk_side_max) << cbpos_z_bit_offset)
		) {}
		constexpr CBPos(const Ushort3& blockPos) noexcept : v(
			((blockPos.x & chunk_side_max) << cbpos_x_bit_offset)
			| ((blockPos.y & chunk_side_max) << cbpos_y_bit_offset)
			| ((blockPos.z & chunk_side_max) << cbpos_z_bit_offset)
		) {}
		constexpr CBPos(const Int3& blockPos) noexcept : v(
			((blockPos.x & chunk_side_max) << cbpos_x_bit_offset)
			| ((blockPos.y & chunk_side_max) << cbpos_y_bit_offset)
			| ((blockPos.z & chunk_side_max) << cbpos_z_bit_offset)
		) {}

		constexpr uchar x() const noexcept {
			return (v & cbpos_x_mask) >> cbpos_x_bit_offset;
		}
		constexpr uchar y() const noexcept {
			return (v & cbpos_y_mask) >> cbpos_y_bit_offset;
		}
		constexpr uchar z() const noexcept {
			return (v & cbpos_z_mask) >> cbpos_z_bit_offset;
		}

		//是否null
		//bool isNull() const;
		//bool notNull() const;
		//将最高位设置为1，表示特殊状态，以用于代表block为null
		//void setNull();
		//移除null状态
		//void removeNull();

		constexpr bool operator ==(const CBPos& a) const noexcept {
			return v == a.v;
		}
		constexpr CBPos operator +(sint a) const noexcept {
			return CBPos(v + a);
		}
		constexpr CBPos& operator +=(sint a) noexcept {
			v += a;
			return *this;
		}
		constexpr CBPos operator -(sint a) const noexcept {
			return CBPos(v - a);
		}
		constexpr CBPos& operator -=(sint a) noexcept {
			v -= a;
			return *this;
		}

		//用于遍历整个区块，坐标遍历顺序为xyz
		constexpr CBPos& operator ++() noexcept {
			++v;
			return *this;
		}
		//用于遍历整个区块，坐标遍历顺序为xyz
		constexpr CBPos operator ++(sint) noexcept {
			CBPos temp = *this;
			++*this;
			return temp;
		}
		//用于遍历整个区块，坐标遍历顺序为xyz
		constexpr CBPos& operator --() noexcept {
			--v;
			return *this;
		}
		//用于遍历整个区块，坐标遍历顺序为xyz
		constexpr CBPos operator --(sint) noexcept {
			CBPos temp = *this;
			--*this;
			return temp;
		}
		inline constexpr operator ushort() const noexcept {
			return v;
		}

		//x值+1，如果越界，则会返回false，并且x值改为0
		constexpr bool xp() noexcept {
			if ((v & cbpos_x_mask) == cbpos_x_mask) {
				v &= cbpos_remove_x_mask;
				return false;
			}
			v += cbpos_x_unit;
			return true;
		}
		//x值-1，如果越界，则会返回false，并且x值改为pos_in_chunk_z_mask
		constexpr bool xn() noexcept {
			if ((v & cbpos_x_mask) == 0) {
				v |= cbpos_x_mask;
				return false;
			}
			v -= cbpos_x_unit;
			return true;
		}
		//y值+1，如果越界，则会返回false，并且y值改为0
		constexpr bool yp() noexcept {
			if ((v & cbpos_y_mask) == cbpos_y_mask) {
				v &= cbpos_remove_y_mask;
				return false;
			}
			v += cbpos_y_unit;
			return true;
		}
		//y值-1，如果越界，则会返回false，并且y值改为pos_in_chunk_z_mask
		constexpr bool yn() noexcept {
			if ((v & cbpos_y_mask) == 0) {
				v |= cbpos_y_mask;
				return false;
			}
			v -= cbpos_y_unit;
			return true;
		}
		//z值+1，如果越界，则会返回false，并且z值改为0
		constexpr bool zp() noexcept {
			if ((v & cbpos_z_mask) == cbpos_z_mask) {
				v &= cbpos_remove_z_mask;
				return false;
			}
			v += cbpos_z_unit;
			return true;
		}
		//z值-1，如果越界，则会返回false，并且z值改为pos_in_chunk_z_mask
		constexpr bool zn() noexcept {
			if ((v & cbpos_z_mask) == 0) {
				v |= cbpos_z_mask;
				return false;
			}
			v -= cbpos_z_unit;
			return true;
		}

		//获取将x值置为0，其他值不变的CBPos
		constexpr CBPos getXpNearChunk() const noexcept {
			return CBPos(v & cbpos_remove_x_mask);
		}
		//获取将x值置为chunk_side_max，其他值不变的CBPos
		constexpr CBPos getXnNearChunk() const noexcept {
			return CBPos(v | cbpos_x_mask);
		}
		//获取将y值置为0，其他值不变的CBPos
		constexpr CBPos getYpNearChunk() const noexcept {
			return CBPos(v & cbpos_remove_y_mask);
		}
		//获取将y值置为chunk_side_max，其他值不变的CBPos
		constexpr CBPos getYnNearChunk() const noexcept {
			return CBPos(v | cbpos_y_mask);
		}
		//获取将z值置为0，其他值不变的CBPos
		constexpr CBPos getZpNearChunk() const noexcept {
			return CBPos(v & cbpos_remove_z_mask);
		}
		//获取将z值置为chunk_side_max，其他值不变的CBPos
		constexpr CBPos getZnNearChunk() const noexcept {
			return CBPos(v | cbpos_z_mask);
		}

		//pos向face方向前进1格，如果越过区块边界，则会返回false，并且将pos改为位于另一个区块的位置
		constexpr bool moveToward(Face3 face) noexcept {
			const uchar shift = face.axis() * chunk_length_bit_length;
			const bool isPositive = face.isPositive();
			const ushort mask = chunk_side_max << shift;
			const ushort boundary = mask * isPositive;
			const sshort unit = (1 << shift) * math::positiveOne(isPositive);

			if ((v & mask) == boundary) {
				v ^= mask;
				return false;
			}
			v += unit;
			return true;
		}

		friend std::ostream& operator<<(std::ostream& os, const CBPos& a) {
			return os << "x: " << tous(a.x()) << ", y: " << tous(a.y()) << ", z: " << tous(a.z());
		}
		friend constexpr Hash Hasher(const CBPos& a) noexcept {
			return Hash::Combine(a.v);
		}
	};

	//区块坐标转为区块起始方块坐标
	constexpr Int3 ToBlockPos(const Int3& chunkPos) noexcept {
		return Int3(
			(chunkPos.x << chunk_length_bit_length),
			(chunkPos.y << chunk_length_bit_length),
			(chunkPos.z << chunk_length_bit_length)
		);
	}
	//区块坐标与区块内方块坐标转为方块坐标
	constexpr Int3 ToBlockPos(const Int3& chunkPos, CBPos pos) noexcept {
		return Int3(
			(chunkPos.x << chunk_length_bit_length) + pos.x(),
			(chunkPos.y << chunk_length_bit_length) + pos.y(),
			(chunkPos.z << chunk_length_bit_length) + pos.z()
		);
	}
	//区块坐标与区块内方块坐标转为方块坐标
	constexpr Int3 ToBlockPos(const Int4& chunkId, CBPos pos) noexcept {
		return Int3(
			(chunkId.x << chunk_length_bit_length) + pos.x(),
			(chunkId.y << chunk_length_bit_length) + pos.y(),
			(chunkId.z << chunk_length_bit_length) + pos.z()
		);
	}
	//方块坐标转为区块坐标
	constexpr Int3 ToChunkPos(const Int3& blockPos) noexcept {
		return Int3(
			blockPos.x >> chunk_length_bit_length,
			blockPos.y >> chunk_length_bit_length,
			blockPos.z >> chunk_length_bit_length
		);
	}
	//方块坐标转为区块内方块坐标
	constexpr CBPos ToCBPos(const Int3& blockPos) noexcept {
		return CBPos(blockPos);
	}

}

namespace std {
	template<>
	struct hash<dme::block::CBPos> {
		size_t operator ()(const dme::block::CBPos& a) const noexcept {
			return Hasher(a);
		}
	};
}