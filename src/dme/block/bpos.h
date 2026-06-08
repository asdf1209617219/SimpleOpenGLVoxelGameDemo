#pragma once
#include <dme/block/block.h>
namespace dme {

	namespace world {
		class World;
	}

	namespace block {
		class Chunk;

		//各种关于方块坐标计算的函数

		//长方体的角坐标转换为方块坐标，direction为该角在长方体的哪个角
		constexpr Int3 CuboidCornerToBlockPos(const Fix3& corner, const Int3& direction) noexcept {
			return Int3(
				corner.xf == 0 && direction.x > 0 ? corner.x - 1 : corner.x,
				corner.yf == 0 && direction.y > 0 ? corner.y - 1 : corner.y,
				corner.zf == 0 && direction.z > 0 ? corner.z - 1 : corner.z
			);
		}
		//其他物体碰撞到方块边上，返回边的坐标，direction为碰撞的方向（1为正向，-1为反向）
		constexpr sint CollideBlockSide(sint blockPos, sint direction) noexcept {
			//return direction > 0 ? blockPos : (blockPos + 1);
			return blockPos + ((direction & 0b10) >> 1);
		}

		//方块位置的封装
		class BPos {
		private:
			Chunk* chunk;
			Block* block;
		public:
			BPos() noexcept;
			BPos(Chunk& chunk, CBPos pos) noexcept;
			BPos(const Int3& blockPos, world::World& world) noexcept;
			BPos(const Int3& blockPos, sid worldId) noexcept;
			explicit BPos(const Int4& blockPos) noexcept;

			bool isNull() const noexcept;
			bool notNull() const noexcept;

			world::World* getWorld() const noexcept;
			Chunk* getChunk() const noexcept;
			Block* getBlock() const noexcept;
			Int3 getBlockPos() const noexcept;
			CBPos getCBPos() const noexcept;
		};
	}
}