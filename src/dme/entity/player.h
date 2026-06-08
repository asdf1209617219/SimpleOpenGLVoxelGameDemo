#pragma once
#include <dme/entity/creature.h>
#include <dme/item/inventory/backpack.h>
#include <dme/block/block.h>

namespace dme {
	namespace world {
		class World;
	}
	namespace graph {
		class CubeFrame;
	}
	namespace phys {
		struct RayTarget;
	}

	namespace entity {
		class Player : public Creature, private ObjPool<Player> {
		private:
			friend ObjPool<Player>;

		protected:
			//const char* uuid;

			uint hotbarIndex; //快捷栏在库存中的起始位置

			void loadChunk(const Int3& oldChunkPos, const Int3& newChunkPos);

			explicit Player(strid name, Fix3 centerPos = Fix3(), world::World* world = null);
		public:
			inline constexpr static float player_half_width = 0.4f;
			inline constexpr static float player_half_height = 0.9f;
			inline constexpr static float player_half_aspect = player_half_height / player_half_width;
			inline constexpr static float player_camera_zoffset = 0.60f;
			inline constexpr static sint player_width_ceil = 1;
			inline constexpr static sint player_height_ceil = 2;
			inline constexpr static float player_jump_velocity_z = 7.0f;
			inline constexpr static float player_max_select_range = 10.0f; //玩家能选取到的方块的范围

			virtual Float3 getCameraPos() const;
			virtual Fix3 getCameraFPos() const;
			uint getHotbarIndex() const;
			void setHotbarIndex(uint hotbarIndex);
			//检查射线碰到的方块，并显示或隐藏选择框
			void checkRay(graph::CubeFrame& cubeFrame, math::Ray3fTarget& rayTarget);
			//将玩家加入世界，加载周围区块，更新物理状态
			virtual void init() override;
			virtual void update() override;
			//获取当前帧要移动的距离
			virtual Float3 getDelta() override;


			//通过对象池创建
			static Player* Create(strid name, Fix3 centerPos = Fix3(), world::World* world = null) noexcept {
				auto ptr = ObjPool<Player>::Add(name, centerPos, world);
				return ptr;
			}
		};

	}
}