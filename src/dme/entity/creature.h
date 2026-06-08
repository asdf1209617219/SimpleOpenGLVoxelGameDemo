#pragma once
#include <dme/entity/entity.h>
#include <dme/item/inventory/backpack.h>
#include <dme/block/block_cuboid.h>
namespace dme {
	namespace world {
		class World;
	}
	namespace entity {
		class Creature : public Entity, private ObjPool<Creature> {
		private:
			friend ObjPool<Creature>;

			//根据碰撞结果设置位置
			void resolveCollisionPos(uint index, sint moveDirection, sint blockPos);
			//根据方块类型决定碰撞结果，返回在pos的位置是否有方块
			bool blockIntersect(
				Int3& pos,
				Bool3& r,
				const block::AABBMove& move
			);
			//查询方块，进行碰撞检测并设置碰撞后的位置
			void blockCollisionResolve(const block::AABBMove& move);
		protected:
			//长宽高的一半
			Float3 half;
			//是否正在跳跃
			bool isJumping;
			//是否正在浮空
			bool isFloating;
			//是否具有速度
			bool hasVelocity;
			//是否飞行
			bool isFly;
			//因受力产生的各个方向的瞬时速度
			Float3 velocity;

			strid name;
			//库存
			item::Inventory inventory;
			//当前手持物品
			uint inHand;

			//摄像机正向量
			Float3 front;
			//摄像机上向量
			Float3 up;

			//移动速度，表示走路的快慢程度（一般是正数）
			float speed;
			//偏航角弧度制(决定左右方向)
			float yaw;
			float yawSin;
			float yawCos;
			//俯仰角弧度制(决定上下方向)
			float pitch;
			float pitchSin;
			float pitchCos;

			explicit Creature(strid name, Fix3 centerPos = Fix3(), Float3 half = Float3(), world::World* world = null, uint inventorySize = 10, float speed = 8.0f);

		public:
			strid getName() const;

			Fix3 getBottomPos() const;
			void setBottomPos(const Fix3& bottomPos);

			virtual math::Abox3f getAbox() const noexcept override;
			//在init前不要使用库存
			virtual void init() override;
			virtual void update() override;
			virtual void triggerPhysics(const Float3& posDelta, const Float3& velocityDelta) override;

			virtual item::Inventory& getInventory();
			//通过改变偏航角俯仰角来改变front和up
			virtual void addYawPitch(float yawDelta, float pitchDelta);
			//获取当前帧要移动的距离
			virtual Float3 getDelta();
			//验证碰撞并设置碰撞后的位置，isTrigger为true则delta为0也会检测碰撞
			void doCollision(const Float3& delta, bool isTrigger = false);

			Float3 getVelocity() const;
			void setVelocity(const Float3& velocity);
			void setVelocityX(float velocityX);
			void setVelocityY(float velocityY);
			void setVelocityZ(float velocityZ);
			void addVelocity(const Float3& velocityDelta);
			void addVelocityX(float velocityXDelta);
			void addVelocityY(float velocityYDelta);
			void addVelocityZ(float velocityZDelta);
			float getSpeed() const;
			void setSpeed(float speed);
			//包装浮空与落地的逻辑
			void setIsFloating(bool isFloating);

			//通过对象池创建
			static Creature* Create(strid name, Fix3 centerPos = Fix3(), Float3 half = Float3(), world::World* world = null, uint inventorySize = 10, float speed = 8.0f) noexcept {
				auto ptr = ObjPool<Creature>::Add(name, centerPos, half, world, inventorySize, speed);
				ptr->updateBVH();
				return ptr;
			}
		};

	}
}