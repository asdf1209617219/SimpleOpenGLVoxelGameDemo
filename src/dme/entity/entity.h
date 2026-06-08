#pragma once
#include <dme/core.h>
namespace dme {
	namespace world {
		class World;
	}
	namespace entity {
		class Entity {
		private:
		protected:
			//实体id，用于ecs系统 // TODO
			uid id;
			//是否需要验证碰撞箱
			bool checkCollision;
			//是否管理员
			bool isAdmin;
			//在bvh中的节点id
			uid node;
			sid worldId;
			world::World* world;
			Hive<Entity*>::comp_ptr entityPoolPtr; //指向保存entity指针的内存池里的指针
			//中心点位置
			Fix3 centerPos;

			//更新bvh节点对应的包围盒
			void updateBVH() noexcept;
			//更新bvh节点对应的包围盒（移动时调用）
			void updateBVH(const Float3& delta) noexcept;
		public:
			//最大瞬时速度
			static constexpr float max_speed = 10000.0f;
			//默认重力系数
			static constexpr float g = 18.0f;

			explicit Entity(Fix3 centerPos = Fix3(), world::World* world = null, bool checkCollision = false);

			//返回中心点位置
			Fix3 getCenterPos() const;
			void setCenterPos(const Fix3& centerPos);
			void setCenterPos(const Fix3& centerPos, sid worldId);
			//获取包围盒
			virtual math::Abox3f getAbox() const noexcept;
			//返回所在世界id
			sid getWorldId() const;
			void setWorldId(sid worldId);
			world::World* getWorld() const;
			//返回所在区块位置
			Int3 getChunkPos() const;
			bool getCheckCollision() const;
			void setCheckCollision(bool checkCollision);
			bool getIsAdmin() const;
			void setIsAdmin(bool isAdmin);

			//实体初始化函数
			virtual void init() = 0;
			//实体每帧更新函数
			virtual void update() = 0;
			//由物理系统触发的物理更新（如更新重力等）// TODO 其实这里将所有具有物理行为的实体抽象为1个接口比较好
			virtual void triggerPhysics(const Float3& posDelta, const Float3& velocityDelta);
		};

	}
}